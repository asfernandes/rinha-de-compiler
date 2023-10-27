#ifndef RINHA_INTERPRETER_ENVIRONMENT_H
#define RINHA_INTERPRETER_ENVIRONMENT_H

#include "./ParsedSource.h"
#include "./Values.h"
#include <iostream>
#include <ostream>
#include <string>

namespace rinha::interpreter
{
	class ParsedSource;

	class Environment
	{
	public:
		static Value run(std::shared_ptr<Environment> environment, std::shared_ptr<ParsedSource> parsedSource);

		virtual void printLine(const std::string& s) = 0;
	};

	class StdEnvironment final : public Environment
	{
	public:
		void printLine(const std::string& s) override
		{
			std::cout << s << std::endl;
		}
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_ENVIRONMENT_H
