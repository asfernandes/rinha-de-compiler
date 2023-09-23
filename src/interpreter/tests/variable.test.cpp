#include "../TestUtil.test.h"
#include <variant>
#include <boost/test/unit_test.hpp>

using namespace rinha::interpreter;


BOOST_AUTO_TEST_SUITE(InterpreterSuite)
BOOST_AUTO_TEST_SUITE(VariableSuite)

BOOST_AUTO_TEST_CASE(isReadCorrectlyFromSameScope)
{
	const auto result = TestUtil::run(R"###(
		let n1 = 1;
		let n2 = "2";
		(n1, n2)
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<IntValue>(tuple.getFirst()).getValue() == 1);
	BOOST_CHECK(std::get<StrValue>(tuple.getSecond()).getValue() == "2");
}

BOOST_AUTO_TEST_CASE(isReadCorrectlyFromDifferentScopes)
{
	const auto result = TestUtil::run(R"###(
		let n1 = 1;
		let f = fn() => {
			let n2 = "2";
			(n1, n2)
		};
		f()
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<IntValue>(tuple.getFirst()).getValue() == 1);
	BOOST_CHECK(std::get<StrValue>(tuple.getSecond()).getValue() == "2");
}

BOOST_AUTO_TEST_CASE(isReadCorrectlyFromPopedScopes)
{
	const auto result = TestUtil::run(R"###(
		let n1 = 1;
		let f = fn() => {
			let n2 = "2";
			fn() => (n1, n2)
		};
		f()()
	)###");

	const auto tuple = std::get<TupleValue>(result.value.value());

	BOOST_CHECK(std::get<IntValue>(tuple.getFirst()).getValue() == 1);
	BOOST_CHECK(std::get<StrValue>(tuple.getSecond()).getValue() == "2");
}

BOOST_AUTO_TEST_CASE(isReadCorrectlyAfterShadow)
{
	const auto result = TestUtil::run(R"###(
		let n1 = 1;
		let n1 = 2;
		n1
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 2);
}

BOOST_AUTO_TEST_CASE(isReadCorrectlyInFnBetweenShadow)
{
	const auto result = TestUtil::run(R"###(
		let n1 = 1;
		let f = fn() => n1;
		let n1 = 2;
		f()
	)###");

	// Weird but ok given hoisting and shadow.
	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 2);
}

BOOST_AUTO_TEST_CASE(readShadowedInSameDeclaration1)
{
	const auto result = TestUtil::run(R"###(
		let n1 = 1;
		let n1 = n1;
		n1
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 1);
}

BOOST_AUTO_TEST_CASE(readShadowedInSameDeclaration2)
{
	const auto result = TestUtil::run(R"###(
		let n1 = 1;
		let f = fn() => {
			let n1 = n1;
			n1
		};
		f()
	)###");

	BOOST_CHECK(std::get<IntValue>(result.value.value()).getValue() == 1);
}

BOOST_AUTO_TEST_SUITE_END()  // VariableSuite
BOOST_AUTO_TEST_SUITE_END()  // InterpreterSuite
