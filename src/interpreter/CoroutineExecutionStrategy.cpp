#include "./CoroutineExecutionStrategy.h"
#include "./Environment.h"
#include "./Nodes.h"
#include "./ParsedSource.h"
#include "./Runtime.h"
#include "./Task.h"
#include "./TermNodeVisitor.h"
#include "./Values.h"
#include <boost/smart_ptr/local_shared_ptr.hpp>

// boost/smart_ptr/local_shared_ptr
using boost::local_shared_ptr;


namespace rinha::interpreter
{
	namespace
	{
		class CoroutineExecuteVisitor final : public TermNodeVisitor<CoroutineExecuteVisitor, Task>
		{
		public:
			Task visitLiteralNode(boost::local_shared_ptr<Context>& context, const LiteralNode* node)
			{
				co_return node->value;
			}

			Task visitTupleNode(boost::local_shared_ptr<Context>& context, const TupleNode* node)
			{
				const auto& firstValue = co_await visit(context, node->first);
				const auto& secondValue = co_await visit(context, node->second);
				co_return TupleValue(firstValue, secondValue);
			}

			Task visitFnNode(boost::local_shared_ptr<Context>& context, const FnNode* node)
			{
				co_return FnValue(node, context);
			}

			Task visitCallNode(boost::local_shared_ptr<Context>& context, const CallNode* node)
			{
				const auto& calleeValue = co_await visit(context, node->callee);

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
						calleeContext->setVariable(parameter->name, co_await visit(context, *argumentIt));
						++argumentIt;
					}

					fnNode->getBody()->compile(calleeContext);

					co_return co_await visit(calleeContext, fnNode->getBody());
				}

				throw RinhaException("Cannot call a non-function.");
			}

			Task visitBinaryOpNode(boost::local_shared_ptr<Context>& context, const BinaryOpNode* node)
			{
				const auto& firstValue = co_await visit(context, node->first);
				const auto& secondValue = co_await visit(context, node->second);

				co_return Runtime::binaryOp(node->op, firstValue, secondValue);
			}

			Task visitIfNode(boost::local_shared_ptr<Context>& context, const IfNode* node)
			{
				const auto& conditionValue = co_await visit(context, node->condition);

				if (const auto conditionValueBool = std::get_if<BoolValue>(&conditionValue))
				{
					if (conditionValueBool->getValue())
						co_return co_await visit(context, node->then);
					else
						co_return co_await visit(context, node->otherwise);
				}

				throw RinhaException("Invalid datatype in if.");
			}

			Task visitTupleIndexNode(boost::local_shared_ptr<Context>& context, const TupleIndexNode* node)
			{
				const auto& value = co_await visit(context, node->arg);

				if (const auto conditionValueTuple = std::get_if<TupleValue>(&value))
					co_return node->index == 0 ? conditionValueTuple->getFirst() : conditionValueTuple->getSecond();

				throw RinhaException("Invalid datatype in tuple function.");
			}

			Task visitVarNode(boost::local_shared_ptr<Context>& context, const VarNode* node)
			{
				co_return context->getVariable(node->reference->name);
			}

			Task visitLetNode(boost::local_shared_ptr<Context>& context, const LetNode* node)
			{
				context->setVariable(node->reference->name, co_await visit(context, node->value));

				co_return co_await visit(context, node->next);
			}

			Task visitPrintNode(boost::local_shared_ptr<Context>& context, const PrintNode* node)
			{
				const auto& value = co_await visit(context, node->arg);

				std::visit([&](auto&& arg) { context->getEnvironment()->printLine(arg.toString()); }, value);

				co_return value;
			}
		};
	}  // namespace

	Value CoroutineExecutionStrategy::run(
		local_shared_ptr<Environment> environment, local_shared_ptr<ParsedSource> parsedSource)
	{
		const auto term = parsedSource->getTerm();

		auto context = make_local_shared<Context>(environment);
		term->compile(context);

		CoroutineExecuteVisitor visitor;
		ManualExecutor executor;

		return executor.syncWait(visitor.visit(context, term));
	}
}  // namespace rinha::interpreter
