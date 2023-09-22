#ifndef RINHA_INTERPRETER_TEST_UTIL_H
#define RINHA_INTERPRETER_TEST_UTIL_H

#include "./Parser.h"
#include "./Context.h"
#include "./Diagnostic.h"
#include <memory>
#include <optional>
#include <string>

namespace rinha::interpreter
{
	struct TestResult
	{
		std::optional<Value> value;
		std::shared_ptr<Diagnostics> diagnostics;
	};

	class TestUtil final
	{
	public:
		static TestResult run(const std::string& source)
		{
			Parser parser(source);
			TestResult result;

			result.diagnostics = parser.getDiagnostics();

			if (!result.diagnostics->hasError())
			{
				const auto parsedSource = parser.getParsedSource();
				const auto term = parsedSource->getTerm();

				const auto context = std::make_shared<Context>();
				term->compile(context);
				result.value = term->execute(context);
			}

			return result;
		}
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_TEST_UTIL_H
