#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(LogicalSuite)

BOOST_AUTO_TEST_CASE(equalInts)
{
	const auto result = TestUtil::run(R"###(
		(4 == 5, 6 == 6)
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<BoolValue>(tuple.getFirst()).getValue() == false);
	BOOST_CHECK(std::get<BoolValue>(tuple.getSecond()).getValue() == true);
}

BOOST_AUTO_TEST_CASE(equalStrs)
{
	const auto result = TestUtil::run(R"###(
		("4" == "5", "6" == "6")
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<BoolValue>(tuple.getFirst()).getValue() == false);
	BOOST_CHECK(std::get<BoolValue>(tuple.getSecond()).getValue() == true);
}

BOOST_AUTO_TEST_CASE(equalBools)
{
	const auto result = TestUtil::run(R"###(
		(false == true, true == true)
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<BoolValue>(tuple.getFirst()).getValue() == false);
	BOOST_CHECK(std::get<BoolValue>(tuple.getSecond()).getValue() == true);
}

BOOST_AUTO_TEST_CASE(notEqualInts)
{
	const auto result = TestUtil::run(R"###(
		(4 != 5, 6 != 6)
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<BoolValue>(tuple.getFirst()).getValue() == true);
	BOOST_CHECK(std::get<BoolValue>(tuple.getSecond()).getValue() == false);
}

BOOST_AUTO_TEST_CASE(notEqualStrs)
{
	const auto result = TestUtil::run(R"###(
		("4" != "5", "6" != "6")
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<BoolValue>(tuple.getFirst()).getValue() == true);
	BOOST_CHECK(std::get<BoolValue>(tuple.getSecond()).getValue() == false);
}

BOOST_AUTO_TEST_CASE(notEqualBools)
{
	const auto result = TestUtil::run(R"###(
		(false != true, true != true)
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<BoolValue>(tuple.getFirst()).getValue() == true);
	BOOST_CHECK(std::get<BoolValue>(tuple.getSecond()).getValue() == false);
}

BOOST_AUTO_TEST_CASE(lessThanInts)
{
	const auto result = TestUtil::run(R"###(
		(4 < 5, 6 < 6)
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<BoolValue>(tuple.getFirst()).getValue() == true);
	BOOST_CHECK(std::get<BoolValue>(tuple.getSecond()).getValue() == false);
}

BOOST_AUTO_TEST_CASE(lessThanOrEqualInts)
{
	const auto result = TestUtil::run(R"###(
		(4 <= 5, 6 <= 6)
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<BoolValue>(tuple.getFirst()).getValue() == true);
	BOOST_CHECK(std::get<BoolValue>(tuple.getSecond()).getValue() == true);
}

BOOST_AUTO_TEST_CASE(greaterThanInts)
{
	const auto result = TestUtil::run(R"###(
		(6 > 5, 6 > 6)
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<BoolValue>(tuple.getFirst()).getValue() == true);
	BOOST_CHECK(std::get<BoolValue>(tuple.getSecond()).getValue() == false);
}

BOOST_AUTO_TEST_CASE(greaterThanOrEqualInts)
{
	const auto result = TestUtil::run(R"###(
		(6 >= 5, 6 >= 6)
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<BoolValue>(tuple.getFirst()).getValue() == true);
	BOOST_CHECK(std::get<BoolValue>(tuple.getSecond()).getValue() == true);
}

BOOST_AUTO_TEST_SUITE_END()  // LogicalSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
