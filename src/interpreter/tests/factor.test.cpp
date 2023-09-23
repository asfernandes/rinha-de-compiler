#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(FactorSuite)

BOOST_AUTO_TEST_CASE(multiply)
{
	const auto result = TestUtil::run(R"###(
		4 * 5
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 20);
}

BOOST_AUTO_TEST_CASE(divide)
{
	const auto result = TestUtil::run(R"###(
		21 / 4
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 5);
}

BOOST_AUTO_TEST_CASE(remainder)
{
	const auto result = TestUtil::run(R"###(
		21 % 4
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 1);
}

BOOST_AUTO_TEST_SUITE_END()  // FactorSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
