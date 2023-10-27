#ifndef RINHA_INTERPRETER_PARSER_H
#define RINHA_INTERPRETER_PARSER_H

#include "./Nodes.h"
#include "./ParsedSource.h"
#include "./Diagnostic.h"
#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <istream>
#include <sstream>
#include <string>

namespace rinha::interpreter
{
	class Parser final
	{
	private:
		struct Hidden;

	public:
		explicit Parser(std::unique_ptr<std::istream> _stream);

		explicit Parser(const std::string& str)
			: Parser(std::make_unique<std::istringstream>(str))
		{
		}

		~Parser();

	public:
		auto getParsedSource() const
		{
			return parsedSource;
		}

		auto getDiagnostics() const
		{
			return diagnostics;
		}

	private:
		std::unique_ptr<std::istream> stream;
		boost::local_shared_ptr<ParsedSource> parsedSource;
		boost::local_shared_ptr<Diagnostics> diagnostics;
		const TermNode* rootTerm = nullptr;
		std::unique_ptr<Hidden> hidden;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_PARSER_H
