#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(IfSuite)

BOOST_AUTO_TEST_CASE(thenValueWhenConditionIsTrue)
{
	const auto result = TestUtil::run(R"###(
		if (true) {
			1
		}
		else {
			2
		}
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 1);
}

BOOST_AUTO_TEST_CASE(otherwiseValueWhenConditionIsFalse)
{
	const auto result = TestUtil::run(R"###(
		if (false) {
			1
		}
		else {
			2
		}
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 2);
}

BOOST_AUTO_TEST_SUITE_END()  // IfSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
