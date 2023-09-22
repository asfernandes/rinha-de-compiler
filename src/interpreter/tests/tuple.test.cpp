#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(TupleSuite)

BOOST_AUTO_TEST_CASE(tupleReturn)
{
	const auto result = TestUtil::run(R"###(
		(1, "one")
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<IntValue>(tuple.getFirst()).getValue() == 1);
	BOOST_CHECK(std::get<StrValue>(tuple.getSecond()).getValue() == "one");
}

BOOST_AUTO_TEST_CASE(first)
{
	const auto result = TestUtil::run(R"###(
		first((1, "one"))
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 1);
}

BOOST_AUTO_TEST_CASE(second)
{
	const auto result = TestUtil::run(R"###(
		second((1, "one"))
	)###");

	BOOST_CHECK(std::get<StrValue>(result.value.value()).getValue() == "one");
}

BOOST_AUTO_TEST_SUITE_END()  // TupleSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
