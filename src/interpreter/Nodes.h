#ifndef RINHA_INTERPRETER_NODES_H
#define RINHA_INTERPRETER_NODES_H

#include "./Context.h"
#include "./Values.h"
#include "./Exceptions.h"
#include "./Environment.h"
#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <boost/smart_ptr/make_local_shared.hpp>
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
		enum class Type : uint8_t
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
		virtual void compile(boost::local_shared_ptr<Context> context) const = 0;
	};

	class LiteralNode final : public TypedNode<TermNode, TermNode::Type::LITERAL>
	{
	public:
		explicit LiteralNode(Value value)
			: value(value)
		{
		}

	public:
		void compile(boost::local_shared_ptr<Context> context) const override { }

	public:
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
		void compile(boost::local_shared_ptr<Context> context) const override
		{
			first->compile(context);
			second->compile(context);
		}

	public:
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
		void compile(boost::local_shared_ptr<Context> context) const override
		{
			std::unordered_set<std::string> set;

			for (const auto& parameter : parameters)
			{
				if (!set.insert(parameter->name).second)
					throw RinhaException("Duplicate parameter '" + parameter->name + "'.");
			}
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

	public:
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
		void compile(boost::local_shared_ptr<Context> context) const override
		{
			callee->compile(context);

			for (const auto argument : arguments)
				argument->compile(context);
		}

	public:
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
		void compile(boost::local_shared_ptr<Context> context) const override
		{
			first->compile(context);
			second->compile(context);
		}

	public:
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
		void compile(boost::local_shared_ptr<Context> context) const override
		{
			condition->compile(context);
			then->compile(context);
			otherwise->compile(context);
		}

	public:
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
		void compile(boost::local_shared_ptr<Context> context) const override
		{
			arg->compile(context);
		}

	public:
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
		void compile(boost::local_shared_ptr<Context> context) const override { }

	public:
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
		void compile(boost::local_shared_ptr<Context> context) const override
		{
			context->createVariable(reference->name);

			value->compile(context);
			next->compile(context);
		}

	public:
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
		void compile(boost::local_shared_ptr<Context> context) const override
		{
			arg->compile(context);
		}

	public:
		const TermNode* const arg;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_NODES_H
