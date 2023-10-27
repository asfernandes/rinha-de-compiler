#ifndef RINHA_INTERPRETER_ENVIRONMENT_H
#define RINHA_INTERPRETER_ENVIRONMENT_H

#include "./ParsedSource.h"
#include "./Values.h"
#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <iostream>
#include <ostream>
#include <string>

namespace rinha::interpreter
{
	class ParsedSource;

	class Environment
	{
	public:
		static Value run(
			boost::local_shared_ptr<Environment> environment, boost::local_shared_ptr<ParsedSource> parsedSource);

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
