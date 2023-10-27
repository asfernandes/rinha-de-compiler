#ifndef RINHA_INTERPRETER_VALUES_H
#define RINHA_INTERPRETER_VALUES_H

#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <boost/smart_ptr/make_local_shared.hpp>
#include <cstdint>
#include <string>
#include <utility>
#include <variant>


namespace rinha::interpreter
{
	class Context;
	class FnNode;

	using Value = std::variant<class BoolValue, class IntValue, class StrValue, class FnValue, class TupleValue>;

	class BoolValue final
	{
	public:
		explicit BoolValue(bool value) noexcept
			: value(value)
		{
		}

		auto getValue() const noexcept
		{
			return value;
		}

		std::string toString() const
		{
			return value ? "true" : "false";
		}

	private:
		bool value;
	};

	class IntValue final
	{
	public:
		explicit IntValue(int32_t value) noexcept
			: value(value)
		{
		}

		auto getValue() const noexcept
		{
			return value;
		}

		std::string toString() const
		{
			return std::to_string(value);
		}

	private:
		int32_t value;
	};

	class StrValue final
	{
	public:
		explicit StrValue(std::string&& value) noexcept
			: value(std::move(value))
		{
		}

		explicit StrValue(const std::string& value)
			: value(value)
		{
		}

		auto getValue() const noexcept
		{
			return value;
		}

		std::string toString() const
		{
			return value;
		}

	private:
		std::string value;
	};

	class FnValue final
	{
	public:
		explicit FnValue(const FnNode* node, boost::local_shared_ptr<Context> context) noexcept
			: node(node),
			  context(std::move(context))
		{
		}

		auto getValue() const noexcept
		{
			return node;
		}

		auto getContext() const noexcept
		{
			return context;
		}

		std::string toString() const
		{
			return "<#closure>";
		}

	private:
		const FnNode* node;
		boost::local_shared_ptr<Context> context;
	};

	class TupleValue final
	{
	public:
		explicit TupleValue(Value&& first, Value&& second)
			: first(boost::make_local_shared<Value>(std::move(first))),
			  second(boost::make_local_shared<Value>(std::move(second)))
		{
		}

		explicit TupleValue(const Value& first, const Value& second)
			: first(boost::make_local_shared<Value>(first)),
			  second(boost::make_local_shared<Value>(second))
		{
		}

		auto getFirst() const noexcept
		{
			return *first;
		}

		auto getSecond() const noexcept
		{
			return *second;
		}

		std::string toString() const
		{
			const auto& firstString = std::visit([](auto&& arg) { return arg.toString(); }, *first);
			const auto& secondString = std::visit([](auto&& arg) { return arg.toString(); }, *second);
			return "(" + firstString + ", " + secondString + ")";
		}

	private:
		boost::local_shared_ptr<Value> first;
		boost::local_shared_ptr<Value> second;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_VALUES_H
