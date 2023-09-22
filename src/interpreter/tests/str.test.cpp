#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(StrSuite)

BOOST_AUTO_TEST_CASE(stringWithoutEscape)
{
	const auto result = TestUtil::run(R"###(
		"123 456"
	)###");

	BOOST_CHECK(std::get<StrValue>(result.value.value()).getValue() == "123 456");
}

BOOST_AUTO_TEST_CASE(stringWithEscapes)
{
	const auto result = TestUtil::run(R"###(
		"12\\34\"56"
	)###");

	BOOST_CHECK(std::get<StrValue>(result.value.value()).getValue() == "12\\34\"56");
}

BOOST_AUTO_TEST_CASE(stringWithSubsequentEscapes)
{
	const auto result = TestUtil::run(R"###(
		"1\\\"2"
	)###");

	BOOST_CHECK(std::get<StrValue>(result.value.value()).getValue() == "1\\\"2");
}

BOOST_AUTO_TEST_SUITE_END()  // StrSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
