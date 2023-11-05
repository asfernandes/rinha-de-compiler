#include "./TreeWalkerExecutionStrategy.h"
#include "./Environment.h"
#include "./Nodes.h"
#include "./ParsedSource.h"
#include "./Runtime.h"
#include "./TermNodeVisitor.h"
#include "./Values.h"
#include <boost/smart_ptr/local_shared_ptr.hpp>

// boost/smart_ptr/local_shared_ptr
using boost::local_shared_ptr;


namespace rinha::interpreter
{
	namespace
	{
		class TreeWalkerExecuteVisitor final : public TermNodeVisitor<TreeWalkerExecuteVisitor, Value>
		{
		public:
			Value visitLiteralNode(boost::local_shared_ptr<Context>& context, const LiteralNode* node)
			{
				return node->value;
			}

			Value visitTupleNode(boost::local_shared_ptr<Context>& context, const TupleNode* node)
			{
				const auto& firstValue = visit(context, node->first);
				const auto& secondValue = visit(context, node->second);
				return TupleValue(firstValue, secondValue);
			}

			Value visitFnNode(boost::local_shared_ptr<Context>& context, const FnNode* node)
			{
				return FnValue(node, context);
			}

			Value visitCallNode(boost::local_shared_ptr<Context>& context, const CallNode* node)
			{
				const auto& calleeValue = visit(context, node->callee);

				if (const auto calleeValueFn = std::get_if<FnValue>(&calleeValue))
				{
					const auto fnNode = calleeValueFn->getValue();

					if (fnNode->getParameters().size() != node->arguments.size())
						throw RinhaException("Arguments and parameters count do not match.");

					auto calleeContext = boost::make_local_shared<Context>(calleeValueFn->getContext());
					auto argumentIt = node->arguments.begin();

					for (const auto parameter : fnNode->getParameters())
					{
						calleeContext->createVariable(parameter->name);
						calleeContext->setVariable(parameter->name, visit(context, *argumentIt));
						++argumentIt;
					}

					fnNode->getBody()->compile(calleeContext);

					return visit(calleeContext, fnNode->getBody());
				}

				throw RinhaException("Cannot call a non-function.");
			}

			Value visitBinaryOpNode(boost::local_shared_ptr<Context>& context, const BinaryOpNode* node)
			{
				const auto& firstValue = visit(context, node->first);
				const auto& secondValue = visit(context, node->second);

				return Runtime::binaryOp(node->op, firstValue, secondValue);
			}

			Value visitIfNode(boost::local_shared_ptr<Context>& context, const IfNode* node)
			{
				const auto& conditionValue = visit(context, node->condition);

				if (const auto conditionValueBool = std::get_if<BoolValue>(&conditionValue))
				{
					if (conditionValueBool->getValue())
						return visit(context, node->then);
					else
						return visit(context, node->otherwise);
				}

				throw RinhaException("Invalid datatype in if.");
			}

			Value visitTupleIndexNode(boost::local_shared_ptr<Context>& context, const TupleIndexNode* node)
			{
				const auto& value = visit(context, node->arg);

				if (const auto conditionValueTuple = std::get_if<TupleValue>(&value))
					return node->index == 0 ? conditionValueTuple->getFirst() : conditionValueTuple->getSecond();

				throw RinhaException("Invalid datatype in tuple function.");
			}

			Value visitVarNode(boost::local_shared_ptr<Context>& context, const VarNode* node)
			{
				return context->getVariable(node->reference->name);
			}

			Value visitLetNode(boost::local_shared_ptr<Context>& context, const LetNode* node)
			{
				context->setVariable(node->reference->name, visit(context, node->value));

				return visit(context, node->next);
			}

			Value visitPrintNode(boost::local_shared_ptr<Context>& context, const PrintNode* node)
			{
				const auto& value = visit(context, node->arg);

				std::visit([&](auto&& arg) { context->getEnvironment()->printLine(arg.toString()); }, value);

				return value;
			}
		};
	}  // namespace

	Value TreeWalkerExecutionStrategy::run(
		local_shared_ptr<Environment> environment, local_shared_ptr<ParsedSource> parsedSource)
	{
		const auto term = parsedSource->getTerm();

		auto context = make_local_shared<Context>(environment);
		term->compile(context);

		TreeWalkerExecuteVisitor visitor;

		return visitor.visit(context, term);
	}
}  // namespace rinha::interpreter
