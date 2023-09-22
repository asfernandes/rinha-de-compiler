#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(IntSuite)

BOOST_AUTO_TEST_CASE(intLiteral)
{
	const auto result = TestUtil::run(R"###(
		5
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 5);
}

BOOST_AUTO_TEST_SUITE_END()  // IntSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
