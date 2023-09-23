#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(PrecedenceSuite)

BOOST_AUTO_TEST_CASE(logical)
{
	// Weird but accordingly to the language definition
	const auto result = TestUtil::run(R"###(
		false && true || true
	)###");

	BOOST_CHECK(std::get<BoolValue>(result.value.value()).getValue() == false);
}

BOOST_AUTO_TEST_CASE(logicalWithParenthesis1)
{
	const auto result = TestUtil::run(R"###(
		(false && true) || true
	)###");

	BOOST_CHECK(std::get<BoolValue>(result.value.value()).getValue() == true);
}

BOOST_AUTO_TEST_CASE(logicalWithParenthesis2)
{
	const auto result = TestUtil::run(R"###(
		false && (true || true)
	)###");

	BOOST_CHECK(std::get<BoolValue>(result.value.value()).getValue() == false);
}

BOOST_AUTO_TEST_CASE(arithmeticAndFactor)
{
	const auto result = TestUtil::run(R"###(
		2 + 3 * 4
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 14);
}

BOOST_AUTO_TEST_CASE(arithmeticAndFactorWithParenthesis)
{
	const auto result = TestUtil::run(R"###(
		(2 + 3) * 4
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 20);
}

BOOST_AUTO_TEST_SUITE_END()  // PrecedenceSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
