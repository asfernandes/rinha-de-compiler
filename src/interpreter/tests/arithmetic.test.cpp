#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(ArithmeticSuite)

BOOST_AUTO_TEST_CASE(sumInts)
{
	const auto result = TestUtil::run(R"###(
		1 + 2
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 3);
}

BOOST_AUTO_TEST_CASE(concatenateStrInt)
{
	const auto result = TestUtil::run(R"###(
		"1" + 2
	)###");

	BOOST_CHECK(std::get<StrValue>(result.value.value()).getValue() == "12");
}

BOOST_AUTO_TEST_CASE(concatenateIntStr)
{
	const auto result = TestUtil::run(R"###(
		10 + "20"
	)###");

	BOOST_CHECK(std::get<StrValue>(result.value.value()).getValue() == "1020");
}

BOOST_AUTO_TEST_CASE(concatenateStrs)
{
	const auto result = TestUtil::run(R"###(
		"11" + "21"
	)###");

	BOOST_CHECK(std::get<StrValue>(result.value.value()).getValue() == "1121");
}

BOOST_AUTO_TEST_SUITE_END()  // ArithmeticSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
