#ifndef RINHA_INTERPRETER_EXECUTION_STRATEGY_H
#define RINHA_INTERPRETER_EXECUTION_STRATEGY_H

#include "./Nodes.h"
#include "./Values.h"
#include <boost/smart_ptr/local_shared_ptr.hpp>

namespace rinha::interpreter
{
	class Environment;
	class ParsedSource;

	class ExecutionStrategy
	{
	public:
		virtual ~ExecutionStrategy() = default;

	public:
		virtual Value run(
			boost::local_shared_ptr<Environment> environment, boost::local_shared_ptr<ParsedSource> parsedSource) = 0;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_EXECUTION_STRATEGY_H
