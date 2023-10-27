#ifndef RINHA_INTERPRETER_VALUES_H
#define RINHA_INTERPRETER_VALUES_H

#include <cstdint>
#include <memory>
#include <string>
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
		explicit FnValue(const FnNode* node, std::shared_ptr<Context> context) noexcept
			: node(node),
			  context(context)
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
		std::shared_ptr<Context> context;
	};

	class TupleValue final
	{
	public:
		explicit TupleValue(Value&& first, Value&& second)
			: first(std::make_shared<Value>(std::move(first))),
			  second(std::make_shared<Value>(std::move(second)))
		{
		}

		explicit TupleValue(const Value& first, const Value& second)
			: first(std::make_shared<Value>(first)),
			  second(std::make_shared<Value>(second))
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
		std::shared_ptr<Value> first;
		std::shared_ptr<Value> second;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_VALUES_H
