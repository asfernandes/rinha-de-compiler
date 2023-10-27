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

			if (const auto literalNode = nodeAs<LiteralNode>(node))
				return self->visitLiteralNode(context, literalNode.value());
			else if (const auto tupleNode = nodeAs<TupleNode>(node))
				return self->visitTupleNode(context, tupleNode.value());
			else if (const auto fnNode = nodeAs<FnNode>(node))
				return self->visitFnNode(context, fnNode.value());
			else if (const auto callNode = nodeAs<CallNode>(node))
				return self->visitCallNode(context, callNode.value());
			else if (const auto binaryOpNode = nodeAs<BinaryOpNode>(node))
				return self->visitBinaryOpNode(context, binaryOpNode.value());
			else if (const auto ifNode = nodeAs<IfNode>(node))
				return self->visitIfNode(context, ifNode.value());
			else if (const auto tupleIndexNode = nodeAs<TupleIndexNode>(node))
				return self->visitTupleIndexNode(context, tupleIndexNode.value());
			else if (const auto varNode = nodeAs<VarNode>(node))
				return self->visitVarNode(context, varNode.value());
			else if (const auto letNode = nodeAs<LetNode>(node))
				return self->visitLetNode(context, letNode.value());
			else if (const auto printNode = nodeAs<PrintNode>(node))
				return self->visitPrintNode(context, printNode.value());
			else
			{
				assert(false);
				throw std::logic_error("Invalid node type for visit");
			}
		}
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_TERM_NODE_VISITOR_H
