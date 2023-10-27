#include "./Environment.h"
#include "./Nodes.h"
#include "./ParsedSource.h"
#include "./Task.h"

// memory
using std::shared_ptr;


namespace rinha::interpreter
{
	Value Environment::run(shared_ptr<Environment> environment, shared_ptr<ParsedSource> parsedSource)
	{
		const auto term = parsedSource->getTerm();

		const auto context = make_shared<Context>(environment);
		term->compile(context);

		ManualExecutor executor;

		return executor.syncWait(term->execute(context));
	}
}  // namespace rinha::interpreter
