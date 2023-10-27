#ifndef RINHA_INTERPRETER_TERM_NODE_VISITOR_H
#define RINHA_INTERPRETER_TERM_NODE_VISITOR_H

#include "./Nodes.h"
#include <boost/smart_ptr/local_shared_ptr.hpp>

namespace rinha::interpreter
{
	class Context;

	template <typename T, typename R>
	concept TermNodeVisitorConcept = requires(T obj) {
		{
			obj.visitLiteralNode(std::declval<boost::local_shared_ptr<Context>&>(), std::declval<const LiteralNode*>())
		} -> std::same_as<R>;

		{
			obj.visitTupleNode(std::declval<boost::local_shared_ptr<Context>&>(), std::declval<const TupleNode*>())
		} -> std::same_as<R>;

		{
			obj.visitFnNode(std::declval<boost::local_shared_ptr<Context>&>(), std::declval<const FnNode*>())
		} -> std::same_as<R>;

		{
			obj.visitCallNode(std::declval<boost::local_shared_ptr<Context>&>(), std::declval<const CallNode*>())
		} -> std::same_as<R>;

		{
			obj.visitBinaryOpNode(
				std::declval<boost::local_shared_ptr<Context>&>(), std::declval<const BinaryOpNode*>())
		} -> std::same_as<R>;

		{
			obj.visitIfNode(std::declval<boost::local_shared_ptr<Context>&>(), std::declval<const IfNode*>())
		} -> std::same_as<R>;

		{
			obj.visitTupleIndexNode(
				std::declval<boost::local_shared_ptr<Context>&>(), std::declval<const TupleIndexNode*>())
		} -> std::same_as<R>;

		{
			obj.visitVarNode(std::declval<boost::local_shared_ptr<Context>&>(), std::declval<const VarNode*>())
		} -> std::same_as<R>;

		{
			obj.visitLetNode(std::declval<boost::local_shared_ptr<Context>&>(), std::declval<const LetNode*>())
		} -> std::same_as<R>;

		{
			obj.visitPrintNode(std::declval<boost::local_shared_ptr<Context>&>(), std::declval<const PrintNode*>())
		} -> std::same_as<R>;
	};

	template <typename This, typename R>
	class TermNodeVisitor
	{
	public:
		virtual ~TermNodeVisitor() = default;

	public:
		R visit(boost::local_shared_ptr<Context>& context, const TermNode* node)
		requires TermNodeVisitorConcept<This, R>
		{
			const auto self = static_cast<This*>(this);

			switch (node->getType())
			{
				case TermNode::Type::LITERAL:
					return self->visitLiteralNode(context, static_cast<const LiteralNode*>(node));

				case TermNode::Type::TUPLE:
					return self->visitTupleNode(context, static_cast<const TupleNode*>(node));

				case TermNode::Type::FN:
					return self->visitFnNode(context, static_cast<const FnNode*>(node));

				case TermNode::Type::CALL:
					return self->visitCallNode(context, static_cast<const CallNode*>(node));

				case TermNode::Type::BINARY_OP:
					return self->visitBinaryOpNode(context, static_cast<const BinaryOpNode*>(node));

				case TermNode::Type::IF:
					return self->visitIfNode(context, static_cast<const IfNode*>(node));

				case TermNode::Type::TUPLE_INDEX:
					return self->visitTupleIndexNode(context, static_cast<const TupleIndexNode*>(node));

				case TermNode::Type::VAR:
					return self->visitVarNode(context, static_cast<const VarNode*>(node));

				case TermNode::Type::LET:
					return self->visitLetNode(context, static_cast<const LetNode*>(node));

				case TermNode::Type::PRINT:
					return self->visitPrintNode(context, static_cast<const PrintNode*>(node));

				default:
					assert(false);

					throw std::logic_error("Invalid node type for visit");
			}
		}
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_TERM_NODE_VISITOR_H
