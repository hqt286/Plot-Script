#include "catch.hpp"

#include "expression.hpp"

TEST_CASE( "Test default expression", "[expression]" ) {

  Expression exp;

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
}

TEST_CASE( "Test double expression", "[expression]" ) {

  Expression exp(6.023);

  REQUIRE(exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
}


TEST_CASE( "Test symbol expression", "[expression]" ) {

  Expression exp(Atom("asymbol"));

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(exp.isHeadSymbol());
  REQUIRE(exp.isTailEmpty());
}

TEST_CASE("Test Head and Tail of Expression", "[expression]")
{
	Expression exp;
	exp.pushback(Expression(Atom(5)));
	exp.pushback(Expression(Atom(2)));
	exp.pushback(Expression(Atom(3)));
	exp.pushback(Expression(Atom(4)));

	Expression exp1;
	exp1.pushback(Expression(Atom(5)));
	exp1.pushback(Expression(Atom(2)));
	exp1.pushback(Expression(Atom(3)));
	exp1.pushback(Expression(Atom(4)));

	Expression exp2;
	exp2.pushback(Expression(Atom(5)));
}

