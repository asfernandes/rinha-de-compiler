#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(BoolSuite)

BOOST_AUTO_TEST_CASE(trueLiteral)
{
	const auto result = TestUtil::run(R"###(
		true
	)###");

	BOOST_CHECK(std::get<BoolValue>(result.value.value()).getValue() == true);
}

BOOST_AUTO_TEST_CASE(falseLiteral)
{
	const auto result = TestUtil::run(R"###(
		false
	)###");

	BOOST_CHECK(std::get<BoolValue>(result.value.value()).getValue() == false);
}

BOOST_AUTO_TEST_SUITE_END()  // BoolSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
