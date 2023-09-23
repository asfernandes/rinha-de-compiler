#include "./Context.h"
#include "./Environment.h"
#include "./Parser.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <utility>

// exception
using std::exception;

// filesystem
namespace fs = std::filesystem;

// fostream
using std::ifstream;

// iostream
using std::cerr;
using std::cout;

// memory
using std::make_shared;

// ostream
using std::endl;

// stdexcept
using std::runtime_error;


namespace rinha::interpreter
{
	static int run(const fs::path& file)
	{
		ifstream stream(file);

		if (stream.fail())
			throw runtime_error("Cannot open " + file.string());

		Parser parser(std::make_unique<ifstream>(std::move(stream)));

		for (const auto& diagnostic : parser.getDiagnostics()->getList())
		{
			cout << "(" << diagnostic.line << ", " << diagnostic.column
				 << "): " << (diagnostic.type == Diagnostic::Type::ERROR ? "Error" : "Warning") << ": "
				 << diagnostic.message << endl;
		}

		if (parser.getDiagnostics()->hasError())
			return 1;

		const auto parsedSource = parser.getParsedSource();
		const auto term = parsedSource->getTerm();

		const auto environment = make_shared<StdEnvironment>();
		const auto context = make_shared<Context>(environment);
		term->compile(context);
		term->execute(context);

		return 0;
	}
}  // namespace rinha::interpreter

int main(int argc, const char* argv[])
{
	using namespace rinha::interpreter;

	try
	{
		if (argc != 2)
		{
			cerr << "Syntax: " << argv[0] << " filename.rinha" << endl;
			return 1;
		}

		return run(argv[1]);
	}
	catch (const exception& ex)
	{
		cerr << "Error: " << ex.what() << endl;
		return 1;
	}
}
