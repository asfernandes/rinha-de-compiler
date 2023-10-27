#include "./EnvVarExecutionStrategy.h"
#include "./CoroutineExecutionStrategy.h"
#include "./TreeWalkerExecutionStrategy.h"
#include "./Environment.h"
#include "./Exceptions.h"
#include "./ParsedSource.h"
#include "./Values.h"
#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <string>
#include <cstdlib>

// boost/smart_ptr/local_shared_ptr
using boost::local_shared_ptr;


namespace rinha::interpreter
{
	Value EnvVarExecutionStrategy::run(
		local_shared_ptr<Environment> environment, local_shared_ptr<ParsedSource> parsedSource)
	{
		const auto env = std::getenv("RINHA_EXEC_STRATEGY");

		if (!env || strcmp(env, "tree-walker") == 0)
			return TreeWalkerExecutionStrategy().run(environment, parsedSource);
		else if (strcmp(env, "coroutine") == 0)
			return CoroutineExecutionStrategy().run(environment, parsedSource);
		else
			throw RinhaException("Unknown execution strategy: " + std::string(env));
	}
}  // namespace rinha::interpreter
