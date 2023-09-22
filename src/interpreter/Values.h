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

	class BoolValue final
	{
	public:
		explicit BoolValue(bool value)
			: value(value)
		{
		}

		bool operator==(const BoolValue& o) const noexcept = default;

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
		explicit IntValue(int32_t value)
			: value(value)
		{
		}

		bool operator==(const IntValue& o) const noexcept = default;

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
		explicit StrValue(const std::string& value)
			: value(value)
		{
		}

		bool operator==(const StrValue& o) const noexcept = default;

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
		explicit FnValue(const FnNode* node, std::shared_ptr<Context> context)
			: node(node),
			  context(context)
		{
		}

		bool operator==(const FnValue& o) const noexcept = default;

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

	using Value = std::variant<BoolValue, IntValue, StrValue, FnValue, class TupleValue>;

	class TupleValue final
	{
	public:
		explicit TupleValue(const Value& first, const Value& second)
			: first(std::make_shared<Value>(first)),
			  second(std::make_shared<Value>(second))
		{
		}

		bool operator==(const TupleValue& o) const noexcept = default;

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
