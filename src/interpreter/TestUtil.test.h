#ifndef RINHA_INTERPRETER_TEST_UTIL_H
#define RINHA_INTERPRETER_TEST_UTIL_H

#include "./CoroutineExecutionStrategy.h"
#include "./Parser.h"
#include "./Context.h"
#include "./Diagnostic.h"
#include "./Environment.test.h"
#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <boost/smart_ptr/make_local_shared.hpp>
#include <optional>
#include <string>

namespace rinha::interpreter
{
	struct TestResult
	{
		boost::local_shared_ptr<TestEnvironment> environment;
		std::optional<Value> value;
		boost::local_shared_ptr<Diagnostics> diagnostics;
	};

	class TestUtil final
	{
	public:
		static TestResult run(const std::string& source)
		{
			Parser parser(source);

			TestResult result;
			result.environment = boost::make_local_shared<TestEnvironment>();

			result.diagnostics = parser.getDiagnostics();

			if (!result.diagnostics->hasError())
			{
				const auto parsedSource = parser.getParsedSource();

				CoroutineExecutionStrategy executionStrategy;
				result.value = executionStrategy.run(result.environment, std::move(parsedSource));
			}

			return result;
		}
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_TEST_UTIL_H
