#ifndef RINHA_INTERPRETER_NODES_H
#define RINHA_INTERPRETER_NODES_H

#include "./Context.h"
#include "./Values.h"
#include "./Exceptions.h"
#include "./Environment.h"
#include "./Task.h"
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <variant>
#include <vector>
#include <cassert>


namespace rinha::interpreter
{
	template <typename T, typename T::Type typeConst>
	class TypedNode : public T
	{
	public:
		using T::T;

	public:
		typename T::Type getType() const override
		{
			return typeConst;
		}

	public:
		constexpr static typename T::Type TYPE = typeConst;
	};


	template <typename To, typename From>
	static std::optional<To*> nodeAs(From* fromNode)
	{
		return fromNode && fromNode->getType() == To::TYPE ? std::optional<To*>(static_cast<To*>(fromNode))
														   : std::optional<To*>();
	}

	template <typename To, typename From>
	static std::optional<const To*> nodeAs(const From* fromNode)
	{
		return fromNode && fromNode->getType() == To::TYPE ? std::optional<const To*>(static_cast<const To*>(fromNode))
														   : std::optional<const To*>();
	}

	template <typename To, typename From>
	static bool nodeIs(const From* fromNode)
	{
		return fromNode && fromNode->getType() == To::TYPE;
	}

	class Node
	{
	public:
		virtual ~Node() = default;

	public:
		unsigned startLine = 0;
		unsigned startColumn = 0;
	};

	class ReferenceNode final : public Node
	{
	public:
		explicit ReferenceNode(const std::string& name)
			: name(name)
		{
		}

	public:
		std::string name;
	};

	class TermNode : public Node
	{
	public:
		enum class Type
		{
			LITERAL,
			TUPLE,
			FN,
			CALL,
			BINARY_OP,
			IF,
			TUPLE_INDEX,
			VAR,
			LET,
			PRINT
		};

	public:
		virtual Type getType() const = 0;
		virtual void compile(std::shared_ptr<Context> context) const = 0;
		virtual Task execute(std::shared_ptr<Context> context) const = 0;
	};

	class LiteralNode final : public TypedNode<TermNode, TermNode::Type::LITERAL>
	{
	public:
		explicit LiteralNode(Value value)
			: value(value)
		{
		}

	public:
		void compile(std::shared_ptr<Context> context) const override { }

		Task execute(std::shared_ptr<Context> context) const override
		{
			co_return value;
		}

	private:
		const Value value;
	};

	class TupleNode final : public TypedNode<TermNode, TermNode::Type::TUPLE>
	{
	public:
		explicit TupleNode(const TermNode* first, const TermNode* second)
			: first(first),
			  second(second)
		{
		}

	public:
		void compile(std::shared_ptr<Context> context) const override
		{
			first->compile(context);
			second->compile(context);
		}

		Task execute(std::shared_ptr<Context> context) const override
		{
			const auto& firstValue = co_await first->execute(context);
			const auto& secondValue = co_await second->execute(context);
			co_return TupleValue(firstValue, secondValue);
		}

	private:
		const TermNode* const first;
		const TermNode* const second;
	};

	class FnNode final : public TypedNode<TermNode, TermNode::Type::FN>
	{
	public:
		explicit FnNode(std::vector<const ReferenceNode*>&& parameters, const TermNode* body)
			: parameters(std::move(parameters)),
			  body(body)
		{
		}

	public:
		void compile(std::shared_ptr<Context> context) const override
		{
			std::unordered_set<std::string> set;

			for (const auto& parameter : parameters)
			{
				if (!set.insert(parameter->name).second)
					throw RinhaException("Duplicate parameter '" + parameter->name + "'.");
			}
		}

		Task execute(std::shared_ptr<Context> context) const override
		{
			co_return FnValue(this, context);
		}

	public:
		auto& getParameters() const noexcept
		{
			return parameters;
		}

		auto getBody() const noexcept
		{
			return body;
		}

	private:
		const std::vector<const ReferenceNode*> parameters;
		const TermNode* const body;
	};

	class CallNode final : public TypedNode<TermNode, TermNode::Type::CALL>
	{
	public:
		explicit CallNode(const TermNode* callee, std::vector<const TermNode*>&& arguments)
			: callee(callee),
			  arguments(std::move(arguments))
		{
		}

	public:
		void compile(std::shared_ptr<Context> context) const override
		{
			callee->compile(context);

			for (const auto argument : arguments)
				argument->compile(context);
		}

		Task execute(std::shared_ptr<Context> context) const override
		{
			const auto& calleeValue = co_await callee->execute(context);

			if (const auto calleeValueFn = std::get_if<FnValue>(&calleeValue))
			{
				const auto fnNode = calleeValueFn->getValue();

				if (fnNode->getParameters().size() != arguments.size())
					throw RinhaException("Arguments and parameters count do not match.");

				const auto calleeContext = std::make_shared<Context>(calleeValueFn->getContext());
				auto argumentIt = arguments.begin();

				for (const auto parameter : fnNode->getParameters())
				{
					calleeContext->createVariable(parameter->name);
					calleeContext->setVariable(parameter->name, co_await (*argumentIt)->execute(context));
					++argumentIt;
				}

				fnNode->getBody()->compile(calleeContext);

				co_return co_await fnNode->getBody()->execute(calleeContext);
			}

			throw RinhaException("Cannot call a non-function.");
		}

	private:
		const TermNode* const callee;
		const std::vector<const TermNode*> arguments;
	};

	class BinaryOpNode final : public TypedNode<TermNode, TermNode::Type::BINARY_OP>
	{
	public:
		enum class Op : uint8_t
		{
			ADD,
			SUB,
			MUL,
			DIV,
			REM,
			EQ,
			NEQ,
			LT,
			GT,
			LTE,
			GTE,
			AND,
			OR,
		};

	public:
		explicit BinaryOpNode(Op op, const TermNode* first, const TermNode* second)
			: op(op),
			  first(first),
			  second(second)
		{
		}

	public:
		void compile(std::shared_ptr<Context> context) const override
		{
			first->compile(context);
			second->compile(context);
		}

		Task execute(std::shared_ptr<Context> context) const override
		{
			// TODO: Short circuit with logical operators.
			const auto& firstValue = co_await first->execute(context);
			const auto& secondValue = co_await second->execute(context);

			switch (op)
			{
				case Op::ADD:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						co_return IntValue(firstInt->getValue() + secondInt->getValue());
					}
					else if ((std::holds_alternative<StrValue>(firstValue) ||
								 std::holds_alternative<IntValue>(firstValue)) &&
						(std::holds_alternative<StrValue>(secondValue) ||
							std::holds_alternative<IntValue>(secondValue)))
					{
						const auto& firstString = std::visit([](auto&& arg) { return arg.toString(); }, firstValue);
						const auto& secondString = std::visit([](auto&& arg) { return arg.toString(); }, secondValue);
						co_return StrValue(firstString + secondString);
					}
					else
						throw RinhaException("Invalid datatypes with operator '+'.");

				case Op::SUB:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						co_return IntValue(firstInt->getValue() - secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '-'.");

				case Op::MUL:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						co_return IntValue(firstInt->getValue() * secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '*'.");

				case Op::DIV:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						co_return IntValue(firstInt->getValue() / secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '/'.");

				case Op::REM:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						co_return IntValue(firstInt->getValue() % secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '%'.");

				case Op::EQ:
				case Op::NEQ:
				case Op::LT:
				case Op::GT:
				case Op::LTE:
				case Op::GTE:
				{
					if (firstValue.index() != secondValue.index())
						throw RinhaException("Cannot compare the two values of different types.");

					std::strong_ordering cmp = std::strong_ordering::equal;

					if (const auto& firstVal = std::get_if<BoolValue>(&firstValue))
						cmp = firstVal->getValue() <=> std::get_if<BoolValue>(&secondValue)->getValue();
					else if (const auto& firstVal = std::get_if<IntValue>(&firstValue))
						cmp = firstVal->getValue() <=> std::get_if<IntValue>(&secondValue)->getValue();
					else if (const auto& firstVal = std::get_if<StrValue>(&firstValue))
						cmp = firstVal->getValue() <=> std::get_if<StrValue>(&secondValue)->getValue();
					else
						throw RinhaException("Invalid datatypes with comparare operator.");

					switch (op)
					{
						case Op::EQ:
							co_return BoolValue(cmp == std::strong_ordering::equal);

						case Op::NEQ:
							co_return BoolValue(cmp != std::strong_ordering::equal);

						case Op::LT:
							co_return BoolValue(cmp == std::strong_ordering::less);

						case Op::GT:
							co_return BoolValue(cmp == std::strong_ordering::greater);

						case Op::LTE:
							co_return BoolValue(cmp != std::strong_ordering::greater);

						case Op::GTE:
							co_return BoolValue(cmp != std::strong_ordering::less);

						default:
							assert(false);
					}

					break;
				}

				case Op::AND:
					if (const auto firstInt = std::get_if<BoolValue>(&firstValue),
						secondInt = std::get_if<BoolValue>(&secondValue);
						firstInt && secondInt)
					{
						co_return BoolValue(firstInt->getValue() && secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '&&'.");

				case Op::OR:
					if (const auto firstInt = std::get_if<BoolValue>(&firstValue),
						secondInt = std::get_if<BoolValue>(&secondValue);
						firstInt && secondInt)
					{
						co_return BoolValue(firstInt->getValue() || secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '||'.");
			}

			assert(false);
			throw std::logic_error("Invalid binary op");
		}

	private:
		const Op op;
		const TermNode* const first;
		const TermNode* const second;
	};

	class IfNode final : public TypedNode<TermNode, TermNode::Type::IF>
	{
	public:
		explicit IfNode(const TermNode* condition, const TermNode* then, const TermNode* otherwise)
			: condition(condition),
			  then(then),
			  otherwise(otherwise)
		{
		}

	public:
		void compile(std::shared_ptr<Context> context) const override
		{
			condition->compile(context);
			then->compile(context);
			otherwise->compile(context);
		}

		Task execute(std::shared_ptr<Context> context) const override
		{
			const auto& conditionValue = co_await condition->execute(context);

			if (const auto conditionValueBool = std::get_if<BoolValue>(&conditionValue))
			{
				if (conditionValueBool->getValue())
					co_return co_await then->execute(context);
				else
					co_return co_await otherwise->execute(context);
			}

			throw RinhaException("Invalid datatype in if.");
		}

	private:
		const TermNode* const condition;
		const TermNode* const then;
		const TermNode* const otherwise;
	};

	class TupleIndexNode final : public TypedNode<TermNode, TermNode::Type::TUPLE_INDEX>
	{
	public:
		explicit TupleIndexNode(const TermNode* arg, unsigned index)
			: arg(arg),
			  index(index)
		{
			if (index != 0 && index != 1)
				throw std::logic_error("Invalid TupleIndex");
		}

	public:
		void compile(std::shared_ptr<Context> context) const override
		{
			arg->compile(context);
		}

		Task execute(std::shared_ptr<Context> context) const override
		{
			const auto& value = co_await arg->execute(context);

			if (const auto conditionValueTuple = std::get_if<TupleValue>(&value))
				co_return index == 0 ? conditionValueTuple->getFirst() : conditionValueTuple->getSecond();

			throw RinhaException("Invalid datatype in tuple function.");
		}

	private:
		const TermNode* const arg;
		const unsigned index;
	};

	class VarNode final : public TypedNode<TermNode, TermNode::Type::VAR>
	{
	public:
		explicit VarNode(const ReferenceNode* reference)
			: reference(reference)
		{
		}

	public:
		void compile(std::shared_ptr<Context> context) const override { }

		Task execute(std::shared_ptr<Context> context) const override
		{
			co_return context->getVariable(reference->name);
		}

	private:
		const ReferenceNode* reference;
	};

	class LetNode final : public TypedNode<TermNode, TermNode::Type::LET>
	{
	public:
		explicit LetNode(const ReferenceNode* reference, const TermNode* value, const TermNode* next)
			: reference(reference),
			  value(value),
			  next(next)
		{
		}

	public:
		void compile(std::shared_ptr<Context> context) const override
		{
			context->createVariable(reference->name);

			value->compile(context);
			next->compile(context);
		}

		Task execute(std::shared_ptr<Context> context) const override
		{
			context->setVariable(reference->name, co_await value->execute(context));

			co_return co_await next->execute(context);
		}

	private:
		const ReferenceNode* reference;
		const TermNode* const value;
		const TermNode* const next;
	};

	class PrintNode final : public TypedNode<TermNode, TermNode::Type::PRINT>
	{
	public:
		explicit PrintNode(const TermNode* arg)
			: arg(arg)
		{
		}

	public:
		void compile(std::shared_ptr<Context> context) const override
		{
			arg->compile(context);
		}

		Task execute(std::shared_ptr<Context> context) const override
		{
			const auto& value = co_await arg->execute(context);

			std::visit([&](auto&& arg) { context->getEnvironment()->printLine(arg.toString()); }, value);

			co_return value;
		}

	private:
		const TermNode* const arg;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_NODES_H
