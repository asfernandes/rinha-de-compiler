#ifndef RINHA_INTERPRETER_COROUTINE_EXECUTION_STRATEGY_H
#define RINHA_INTERPRETER_COROUTINE_EXECUTION_STRATEGY_H

#include "./ExecutionStrategy.h"

namespace rinha::interpreter
{
	class CoroutineExecutionStrategy final : public ExecutionStrategy
	{
	public:
		Value run(boost::local_shared_ptr<Environment> environment,
			boost::local_shared_ptr<ParsedSource> parsedSource) override;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_COROUTINE_EXECUTION_STRATEGY_H
