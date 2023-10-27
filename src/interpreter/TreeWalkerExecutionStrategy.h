#ifndef RINHA_INTERPRETER_TREE_WALKER_EXECUTION_STRATEGY_H
#define RINHA_INTERPRETER_TREE_WALKER_EXECUTION_STRATEGY_H

#include "./ExecutionStrategy.h"

namespace rinha::interpreter
{
	class TreeWalkerExecutionStrategy final : public ExecutionStrategy
	{
	public:
		Value run(boost::local_shared_ptr<Environment> environment,
			boost::local_shared_ptr<ParsedSource> parsedSource) override;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_TREE_WALKER_EXECUTION_STRATEGY_H
