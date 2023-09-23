#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(PrintSuite)

BOOST_AUTO_TEST_CASE(printInt)
{
	const auto result = TestUtil::run(R"###(
		print(1)
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 1);
	BOOST_CHECK(result.environment->getLines().size() == 1);
	BOOST_CHECK(result.environment->getLines()[0] == "1");
}

BOOST_AUTO_TEST_CASE(printStr)
{
	const auto result = TestUtil::run(R"###(
		print("a")
	)###");

	BOOST_CHECK(std::get<StrValue>(result.value.value()).getValue() == "a");
	BOOST_CHECK(result.environment->getLines().size() == 1);
	BOOST_CHECK(result.environment->getLines()[0] == "a");
}

BOOST_AUTO_TEST_CASE(printBool)
{
	const auto result = TestUtil::run(R"###(
		print(true)
	)###");

	BOOST_CHECK(std::get<BoolValue>(result.value.value()).getValue() == true);
	BOOST_CHECK(result.environment->getLines().size() == 1);
	BOOST_CHECK(result.environment->getLines()[0] == "true");
}

BOOST_AUTO_TEST_CASE(printTuple)
{
	const auto result = TestUtil::run(R"###(
		print(("a", 1))
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<StrValue>(tuple.getFirst()).getValue() == "a");
	BOOST_CHECK(std::get<IntValue>(tuple.getSecond()).getValue() == 1);
	BOOST_CHECK(result.environment->getLines().size() == 1);
	BOOST_CHECK(result.environment->getLines()[0] == "(a, 1)");
}

BOOST_AUTO_TEST_CASE(printFn)
{
	const auto result = TestUtil::run(R"###(
		print(fn() => 1)
	)###");

	BOOST_CHECK(std::holds_alternative<FnValue>(result.value.value()));

	BOOST_CHECK(result.environment->getLines().size() == 1);
	BOOST_CHECK(result.environment->getLines()[0] == "<#closure>");
}

BOOST_AUTO_TEST_SUITE_END()  // PrintSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
