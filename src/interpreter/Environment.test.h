#ifndef RINHA_INTERPRETER_TEST_ENVIRONMENT_H
#define RINHA_INTERPRETER_TEST_ENVIRONMENT_H

#include "./Environment.h"
#include <string>
#include <vector>

namespace rinha::interpreter
{
	class TestEnvironment final : public Environment
	{
	public:
		void printLine(const std::string& s) override
		{
			lines.push_back(s);
		}

		const auto& getLines() const noexcept
		{
			return lines;
		}

	private:
		std::vector<std::string> lines;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_TEST_ENVIRONMENT_H
