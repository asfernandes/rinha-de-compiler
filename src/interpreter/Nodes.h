#ifndef RINHA_INTERPRETER_NODES_H
#define RINHA_INTERPRETER_NODES_H

#include "./Context.h"
#include "./Values.h"
#include "./Exceptions.h"
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
		virtual Value execute(std::shared_ptr<Context> context) const = 0;
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

		Value execute(std::shared_ptr<Context> context) const override
		{
			return value;
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

		Value execute(std::shared_ptr<Context> context) const override
		{
			const auto& firstValue = first->execute(context);
			const auto& secondValue = second->execute(context);
			return TupleValue(firstValue, secondValue);
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

		Value execute(std::shared_ptr<Context> context) const override
		{
			return FnValue(this, context);
		}

	public:
		const auto& getParameters() const noexcept
		{
			return parameters;
		}

		const auto& getBody() const noexcept
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

		Value execute(std::shared_ptr<Context> context) const override
		{
			const auto& calleeValue = callee->execute(context);

			if (const auto calleeValueFn = std::get_if<FnValue>(&calleeValue))
			{
				const auto fnNode = calleeValueFn->getValue();

				if (fnNode->getParameters().size() != arguments.size())
					throw RinhaException("Arguments and parameters count do not match.");

				const auto calleeContext = std::make_shared<Context>(calleeValueFn->getContext());
				auto argumentIt = arguments.begin();

				for (const auto& parameter : fnNode->getParameters())
				{
					calleeContext->createVariable(parameter->name);
					calleeContext->setVariable(parameter->name, (*argumentIt)->execute(context));
					++argumentIt;
				}

				fnNode->getBody()->compile(calleeContext);

				return fnNode->getBody()->execute(calleeContext);
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

		Value execute(std::shared_ptr<Context> context) const override
		{
			// TODO: Short circuit with logical operators.
			const auto& firstValue = first->execute(context);
			const auto& secondValue = second->execute(context);

			switch (op)
			{
				case Op::ADD:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						return IntValue(firstInt->getValue() + secondInt->getValue());
					}
					else
					{
						const auto& firstString = std::visit([](auto&& arg) { return arg.toString(); }, firstValue);
						const auto& secondString = std::visit([](auto&& arg) { return arg.toString(); }, secondValue);
						return StrValue(firstString + secondString);
					}

				case Op::SUB:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						return IntValue(firstInt->getValue() - secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes in subtraction.");

				case Op::MUL:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						return IntValue(firstInt->getValue() * secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes in multiplication.");

				case Op::DIV:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						return IntValue(firstInt->getValue() / secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes in division.");

				case Op::REM:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						return IntValue(firstInt->getValue() % secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes in remainder.");

				case Op::EQ:
					return BoolValue(firstValue == secondValue);

				case Op::NEQ:
					return BoolValue(firstValue != secondValue);

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
						return BoolValue(false);

					switch (op)
					{
						case Op::LT:
							return BoolValue(cmp == std::strong_ordering::less);

						case Op::GT:
							return BoolValue(cmp == std::strong_ordering::greater);

						case Op::LTE:
							return BoolValue(cmp != std::strong_ordering::greater);

						case Op::GTE:
							return BoolValue(cmp != std::strong_ordering::less);

						default:
							assert(false);
					}

					if (const auto& firstVal = std::get_if<BoolValue>(&firstValue))
						return BoolValue(firstVal->getValue() < std::get_if<BoolValue>(&secondValue)->getValue());
					else if (const auto& firstVal = std::get_if<IntValue>(&firstValue))
						return BoolValue(firstVal->getValue() < std::get_if<IntValue>(&secondValue)->getValue());
					else if (const auto& firstVal = std::get_if<StrValue>(&firstValue))
						return BoolValue(firstVal->getValue() < std::get_if<StrValue>(&secondValue)->getValue());

					return BoolValue(false);
				}

				case Op::AND:
					if (const auto firstInt = std::get_if<BoolValue>(&firstValue),
						secondInt = std::get_if<BoolValue>(&secondValue);
						firstInt && secondInt)
					{
						return BoolValue(firstInt->getValue() && secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes in subtraction.");

				case Op::OR:
					if (const auto firstInt = std::get_if<BoolValue>(&firstValue),
						secondInt = std::get_if<BoolValue>(&secondValue);
						firstInt && secondInt)
					{
						return BoolValue(firstInt->getValue() || secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes in subtraction.");
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

		Value execute(std::shared_ptr<Context> context) const override
		{
			const auto& conditionValue = condition->execute(context);

			if (const auto conditionValueBool = std::get_if<BoolValue>(&conditionValue))
				return conditionValueBool->getValue() ? then->execute(context) : otherwise->execute(context);

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

		Value execute(std::shared_ptr<Context> context) const override
		{
			const auto& value = arg->execute(context);

			if (const auto conditionValueTuple = std::get_if<TupleValue>(&value))
				return index == 0 ? conditionValueTuple->getFirst() : conditionValueTuple->getSecond();

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

		Value execute(std::shared_ptr<Context> context) const override
		{
			return context->getVariable(reference->name);
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

		Value execute(std::shared_ptr<Context> context) const override
		{
			context->setVariable(reference->name, value->execute(context));

			return next->execute(context);
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

		Value execute(std::shared_ptr<Context> context) const override
		{
			const auto& value = arg->execute(context);

			std::visit([&](auto&& arg) { context->getEnvironment()->printLine(arg.toString()); }, value);

			return value;
		}

	private:
		const TermNode* const arg;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_NODES_H
