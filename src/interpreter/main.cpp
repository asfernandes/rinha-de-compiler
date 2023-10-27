#include "./Environment.h"
#include "./ParsedSource.h"
#include "./Parser.h"
#include <boost/smart_ptr/make_local_shared.hpp>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <utility>

// boost/smart_ptr/make_local_shared
using boost::make_local_shared;

// exception
using std::exception;

// filesystem
namespace fs = std::filesystem;

// fostream
using std::ifstream;

// iostream
using std::cerr;
using std::cout;

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
		const auto environment = make_local_shared<StdEnvironment>();
		Environment::run(std::move(environment), std::move(parsedSource));

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
