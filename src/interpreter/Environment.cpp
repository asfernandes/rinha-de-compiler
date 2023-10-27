#include "./Environment.h"
#include "./Nodes.h"
#include "./ParsedSource.h"
#include "./Task.h"
#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <boost/smart_ptr/make_local_shared.hpp>

// boost/smart_ptr/local_shared_ptr
using boost::local_shared_ptr;

// boost/smart_ptr/make_local_shared
using boost::make_local_shared;


namespace rinha::interpreter
{
	Value Environment::run(local_shared_ptr<Environment> environment, local_shared_ptr<ParsedSource> parsedSource)
	{
		const auto term = parsedSource->getTerm();

		const auto context = make_local_shared<Context>(environment);
		term->compile(context);

		ManualExecutor executor;

		return executor.syncWait(term->execute(context));
	}
}  // namespace rinha::interpreter
