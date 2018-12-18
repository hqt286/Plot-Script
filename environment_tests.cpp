#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"

#include <cmath>
typedef std::vector<Expression> VectorExpression;
TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));
  REQUIRE(env.is_known(Atom("e")));
  REQUIRE(env.is_exp(Atom("e")));
  REQUIRE(env.is_known(Atom("I")));
  REQUIRE(env.is_exp(Atom("I")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  REQUIRE(env.is_proc(Atom("+")));
  REQUIRE(env.is_proc(Atom("-")));
  REQUIRE(env.is_proc(Atom("*")));
  REQUIRE(env.is_proc(Atom("/")));
  REQUIRE(!env.is_proc(Atom("op")));
}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom("pi")) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());

  REQUIRE(env.get_exp(Atom("I")) == Expression(std::complex<double> (0,1)));
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  REQUIRE(env.is_known(Atom("one")));
  REQUIRE(env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == a);

  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);
  REQUIRE(env.is_known(Atom("hi")));
  REQUIRE(env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == b);

  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE( "Test get built-in procedure", "[environment]" ) {
  Environment env;

  INFO("default procedure")
  Procedure p1 = env.get_proc(Atom("doesnotexist"));
  Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
  REQUIRE(p1 == p2);
  std::vector<Expression> args;
  REQUIRE(p1(args) == Expression());
  REQUIRE(p2(args) == Expression());

  INFO("trying add procedure")
  Procedure padd = env.get_proc(Atom("+"));
  args.emplace_back(1.0);
  args.emplace_back(2.0);
  REQUIRE(padd(args) == Expression(3.0));
}

TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);

  env.reset();
  REQUIRE(!env.is_known(Atom("one")));
  REQUIRE(!env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == Expression());
  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test semeantic errors", "[environment]" ) {

  Environment env;
  {
    Expression exp(Atom("begin"));
    
    REQUIRE_THROWS_AS(exp.eval(env), SemanticError);
	
  }

}

TEST_CASE("Test get all built-in procedure for number and complex number", "[environment]") {
	Environment env;
	Procedure addproc = env.get_proc(Atom("+"));
	Procedure subproc = env.get_proc(Atom("-"));
	Procedure mulproc = env.get_proc(Atom("*"));
	Procedure divproc = env.get_proc(Atom("/"));
	Procedure listproc = env.get_proc(Atom("list"));
	Procedure sqrtproc = env.get_proc(Atom("sqrt"));
	Procedure powproc = env.get_proc(Atom("^"));
	Procedure ln = env.get_proc(Atom("ln"));
	Procedure sine = env.get_proc(Atom("sin"));
	Procedure cosine = env.get_proc(Atom("cos"));
	Procedure tangent = env.get_proc(Atom("tan"));
	Procedure realnumber = env.get_proc(Atom("real"));
	Procedure imagnumber = env.get_proc(Atom("imag"));
	Procedure magnitude = env.get_proc(Atom("mag"));
	Procedure argument = env.get_proc(Atom("arg"));
	Procedure conjugate = env.get_proc(Atom("conj"));


	INFO("Adding 1 positive number and 1 complex I.")
		VectorExpression args1;
		args1.emplace_back(Expression(Atom((2))));
		args1.emplace_back(Expression(env.get_exp(Atom("I"))));
		REQUIRE(addproc(args1) == Expression(Atom(ComplexNumber(2, 1))));
		Expression List = listproc(args1);
		args1.emplace_back(List);
		REQUIRE_THROWS_AS(addproc(args1), SemanticError);

	INFO("Adding 2 real positive numbers together.")
		VectorExpression args1_1;
		args1_1.emplace_back(Expression(Atom((2))));
		args1_1.emplace_back(Expression(Atom((3))));
		REQUIRE(addproc(args1_1) == Expression(Atom(5)));
		args1_1.emplace_back(List);
		REQUIRE_THROWS_AS(addproc(args1_1), SemanticError);

	INFO("Adding 1 negative number and 1 complex I.")
		VectorExpression args1_2;
		args1_2.emplace_back(Expression(Atom((-2))));
		args1_2.emplace_back(Expression(env.get_exp(Atom("I"))));
		REQUIRE(addproc(args1_2) == Expression(Atom(ComplexNumber(-2, 1))));

	INFO("Adding 2 positive numbers and 1 complex I.")
		VectorExpression args1_3;
		args1_3.emplace_back(Expression(Atom((2))));
		args1_3.emplace_back(Expression(Atom((3))));
		args1_3.emplace_back(Expression(env.get_exp(Atom("I"))));
		REQUIRE(addproc(args1_3) == Expression(Atom(ComplexNumber(5, 1))));

	INFO("Adding 2 negative numbers and 1 complex I.")
		VectorExpression args1_4;
		args1_4.emplace_back(Expression(Atom((-2))));
		args1_4.emplace_back(Expression(Atom((-3))));
		args1_4.emplace_back(Expression(env.get_exp(Atom("I"))));
		REQUIRE(addproc(args1_4) == Expression(Atom(ComplexNumber(-5, 1))));

	INFO("Adding 1 positive numbers and 2 complex I.")
		VectorExpression args1_5;
		args1_5.emplace_back(Expression(Atom((2))));
		args1_5.emplace_back(Expression(env.get_exp(Atom("I"))));
		args1_5.emplace_back(Expression(env.get_exp(Atom("I"))));
		REQUIRE(addproc(args1_5) == Expression(Atom(ComplexNumber(2, 2))));

	INFO("Substracting 1 positive number and 1 complex I.")
		VectorExpression args2;
		args2.emplace_back(Expression(Atom((2))));
		args2.emplace_back(Expression(env.get_exp(Atom("I"))));
		REQUIRE(subproc(args2) == Expression(Atom(ComplexNumber(2, -1))));

	INFO("subtracting 1 negative and a list.")
		VectorExpression args2_1;
		args2_1.emplace_back(Expression(Atom((-2))));
		args2_1.emplace_back(List);
		REQUIRE_THROWS_AS(subproc(args2_1), SemanticError);

	INFO("Substracting 1 complex number and 1 list.")
		VectorExpression args2_2;
		args2_2.emplace_back(Expression(env.get_exp(Atom("I"))));
		args2_2.emplace_back(List);
		REQUIRE_THROWS_AS(subproc(args2_2), SemanticError);

	INFO("Substracting 2 complex numbers.")
		VectorExpression args2_3;
		args2_3.emplace_back(Expression(env.get_exp(Atom("I"))));
		args2_3.emplace_back(Expression(env.get_exp(Atom("I"))));
		REQUIRE(subproc(args2_3) == Expression(Atom(ComplexNumber(0, 0))));

	INFO("Substracting 1 complex and 1 numbers.")
		VectorExpression args2_4;
		args2_4.emplace_back(Expression(env.get_exp(Atom("I"))));
		args2_4.emplace_back(Expression(Atom((2))));
		REQUIRE(subproc(args2_4) == Expression(Atom(ComplexNumber(-2, 1))));

	INFO("Substracting 1 complex and 1 numbers and 1 number.")
		VectorExpression args2_5;
		args2_5.emplace_back(Expression(env.get_exp(Atom("I"))));
		args2_5.emplace_back(Expression(Atom((2))));
		args2_5.emplace_back(Expression(Atom((2))));
		REQUIRE_THROWS_AS(subproc(args2_5), SemanticError);

	INFO("Negating a list.")
		VectorExpression args2_6;
		args2_6.emplace_back(List);
		REQUIRE_THROWS_AS(subproc(args2_6), SemanticError);

	INFO("Negating a complex number.")
		VectorExpression args2_7;
		args2_7.emplace_back(Expression(env.get_exp(Atom("I"))));
		REQUIRE(subproc(args2_7) == Expression(Atom(ComplexNumber(-0, -1))));

	INFO("multiply a positive number with I.")
		VectorExpression args3;
		args3.emplace_back(Expression(Atom((2))));
		args3.emplace_back(Expression(env.get_exp(Atom("I"))));
		args3.emplace_back(Expression(env.get_exp(Atom("I"))));
		REQUIRE(mulproc(args3) == Expression(Atom(ComplexNumber(-2, 0))));
		args3.emplace_back(List);

	INFO("multiply a positive number with I with a list.")
		REQUIRE_THROWS_AS(mulproc(args3), SemanticError);

	INFO("multiply a positive number with a positive number.")
		VectorExpression args3_1;
		args3_1.emplace_back(Expression(Atom((2))));
		args3_1.emplace_back(Expression(Atom((6))));
	INFO("multiply a positive number with a positive number with a list.")
		args3_1.emplace_back(List);
		REQUIRE_THROWS_AS(mulproc(args3_1), SemanticError);

	INFO("Divide a number with a list")
		VectorExpression args4;
		args4.emplace_back(Expression(Atom((2))));
		args4.emplace_back(List);
		REQUIRE_THROWS_AS(divproc(args4), SemanticError);

	INFO("Divide 3 different numbers")
		VectorExpression args4_1;
		args4_1.emplace_back(Expression(Atom((2))));
		args4_1.emplace_back(Expression(Atom((2))));
		args4_1.emplace_back(Expression(Atom((2))));
		REQUIRE_THROWS_AS(divproc(args4_1), SemanticError);

	INFO("Divide 2 complex numbers")
		VectorExpression args4_2;
		args4_2.emplace_back(Expression(Atom(ComplexNumber(-2, 4))));
		args4_2.emplace_back(Expression(Atom(ComplexNumber(-2, 2))));
		REQUIRE(divproc(args4_2) == Expression(Atom(ComplexNumber(1.5, -0.5))));

	INFO("Divide 1 complex to 1 real numbers")
		VectorExpression args4_3;
		args4_3.emplace_back(Expression(Atom(ComplexNumber(-2, 4))));
		args4_3.emplace_back(Expression(Atom((2))));
		REQUIRE(divproc(args4_3) == Expression(Atom(ComplexNumber(-1, 2))));

	INFO("Divide 1 real numbers to 1 complex ")
		VectorExpression args4_4;
		args4_4.emplace_back(Expression(Atom((2))));
		args4_4.emplace_back(Expression(Atom(ComplexNumber(-2, 4))));
		REQUIRE(divproc(args4_4) == Expression(Atom(ComplexNumber(-0.2, -0.4))));

	INFO("Divide 1 list to 1 complex ")
		VectorExpression args4_5;
		args4_5.emplace_back(List);
		args4_5.emplace_back(Expression(Atom(ComplexNumber(-2, 4))));
		REQUIRE_THROWS_AS(divproc(args4_5), SemanticError);

	INFO("Divide 1 complex to 2 numbers ")
		VectorExpression args4_6;
		args4_6.emplace_back(Expression(Atom(ComplexNumber(-2, 4))));
		args4_6.emplace_back(Expression(Atom((2))));
		args4_6.emplace_back(Expression(Atom((2))));
		REQUIRE_THROWS_AS(divproc(args4_6), SemanticError);

	INFO("Inverse 1 complex number ")
		VectorExpression args4_7;
		args4_7.emplace_back(Expression(Atom(ComplexNumber(0, 1))));
		REQUIRE(divproc(args4_7) == Expression(Atom(ComplexNumber(0, -1))));

	INFO("Inverse 1 number ")
		VectorExpression args4_8;
		args4_8.emplace_back(Expression(Atom(4)));
		REQUIRE(divproc(args4_8) == Expression(Atom(0.25)));

	INFO("Inverse none number ")
		VectorExpression args4_9;
		args4_9.emplace_back(Expression(Atom("list")));
		REQUIRE_THROWS_AS(divproc(args4_9), SemanticError);

	INFO("Get square root of a real non negative number")
		VectorExpression args5;
		args5.emplace_back(Expression(Atom((4))));
		REQUIRE(sqrtproc(args5) == Expression(Atom(2)));

	INFO("Get square root of a real, negative number")
		VectorExpression args5_1;
		args5_1.emplace_back(Expression(Atom((-4))));
		REQUIRE(sqrtproc(args5_1) == Expression(Atom(ComplexNumber(0, 2))));

	INFO("Get square root of a complex number")
		VectorExpression args5_2;
		args5_2.emplace_back(Expression(Atom(ComplexNumber(8, -6))));
		REQUIRE(sqrtproc(args5_2) == Expression(Atom(ComplexNumber(3, -1))));

	INFO("Get square root of a list")
		VectorExpression args5_3;
		args5_3.emplace_back(List);
		REQUIRE_THROWS_AS(sqrtproc(args5_3), SemanticError);

	INFO("Get square root of 2 different numbers")
		VectorExpression args5_4;
		args5_4.emplace_back(Expression(Atom((-4))));
		args5_4.emplace_back(Expression(Atom((5))));
		REQUIRE_THROWS_AS(sqrtproc(args5_4), SemanticError);

	INFO("Get power of 2 real numbers")
		VectorExpression args6;
		args6.emplace_back(Expression(Atom((-4))));
		args6.emplace_back(Expression(Atom((5))));
		REQUIRE(powproc(args6) == Expression(Atom(-1024)));

	INFO("A complex number to a real number")
		VectorExpression args6_1;
		args6_1.emplace_back(Expression(Atom(ComplexNumber(4, 5))));
		args6_1.emplace_back(Expression(Atom((2))));
		REQUIRE(powproc(args6_1).head().operator==(Expression(Atom(ComplexNumber(-9, 40))).head()));

	INFO("A real number to a complex number")
		VectorExpression args6_2;
	    args6_2.emplace_back(Expression(Atom((2))));
		args6_2.emplace_back(Expression(Atom(ComplexNumber(5.5, 4.2))));
		REQUIRE(powproc(args6_2) == Expression(Atom(ComplexNumber(-44.0592443, 10.3335857))));

	INFO("A complex number to a complex number")
		VectorExpression args6_3;
		args6_3.emplace_back(Expression(Atom(ComplexNumber(2, 3))));
		args6_3.emplace_back(Expression(Atom(ComplexNumber(4, 1))));
		REQUIRE(powproc(args6_3) == Expression(Atom(ComplexNumber(30.3939128, -55.4693743))));
		
	INFO("A complex number to a list")
		VectorExpression args6_4;
		args6_4.emplace_back(Expression(Atom(ComplexNumber(2, 3))));
		args6_4.emplace_back(List);
		REQUIRE_THROWS_AS(powproc(args6_4), SemanticError);

	INFO("A complex number to a list")
		VectorExpression args6_5;
		args6_5.emplace_back(Expression(Atom(ComplexNumber(2, 3))));
		args6_5.emplace_back(Expression(Atom(2)));
		args6_5.emplace_back(Expression(Atom(3)));
		REQUIRE_THROWS_AS(powproc(args6_5), SemanticError);

	INFO("Nature Log of a number")
		VectorExpression args7;
		args7.emplace_back(Expression(Atom(14)));
		REQUIRE(ln(args7) == Expression(Atom(2.63905732962)));

	INFO("Nature Log of a list")
		VectorExpression args7_1;
		args7_1.emplace_back(List);
		REQUIRE_THROWS_AS(ln(args7_1), SemanticError);

	INFO("Nature Log of 2 numbers")
		VectorExpression args7_2;
		args7_2.emplace_back(Expression(Atom(2)));
		args7_2.emplace_back(Expression(Atom(3)));
		REQUIRE_THROWS_AS(ln(args7_2), SemanticError);

	INFO("Nature Log of a negative numbers")
		VectorExpression args7_3;
		args7_3.emplace_back(Expression(Atom(-2)));
		REQUIRE_THROWS_AS(ln(args7_3), SemanticError);

	INFO("Sine of a number")
		VectorExpression args8;
		args8.emplace_back(Expression(Atom(5)));
		REQUIRE(sine(args8) == sine(args8));

	INFO("Sine of 2 numbers")
		VectorExpression args8_1;
		args8_1.emplace_back(Expression(Atom(5)));
		args8_1.emplace_back(Expression(Atom(6)));
		REQUIRE_THROWS_AS(sine(args8_1), SemanticError);

	INFO("Sine of a non-number")
		VectorExpression args8_2;
		args8_2.emplace_back(List);
		REQUIRE_THROWS_AS(sine(args8_2), SemanticError);

	INFO("Cosine of a number")
		VectorExpression args9;
		args9.emplace_back(Expression(Atom(5)));
		REQUIRE(cosine(args9) == cosine(args9));

	INFO("Cosine of 2 numbers")
		VectorExpression args9_1;
		args9_1.emplace_back(Expression(Atom(5)));
		args9_1.emplace_back(Expression(Atom(6)));
		REQUIRE_THROWS_AS(cosine(args9_1), SemanticError);

	INFO("Sine of a non-number")
		VectorExpression args9_2;
		args9_2.emplace_back(List);
		REQUIRE_THROWS_AS(cosine(args9_2), SemanticError);

	INFO("Tangent of a number")
		VectorExpression args10;
		args10.emplace_back(Expression(Atom(5)));
		REQUIRE(tangent(args10) == tangent(args10));

	INFO("tangent of 2 numbers")
		VectorExpression args10_1;
		args10_1.emplace_back(Expression(Atom(5)));
		args10_1.emplace_back(Expression(Atom(6)));
		REQUIRE_THROWS_AS(tangent(args10_1), SemanticError);

	INFO("tangent of a non-number")
		VectorExpression args10_2;
		args10_2.emplace_back(List);
		REQUIRE_THROWS_AS(tangent(args10_2), SemanticError);

	INFO("real of a non complex number")
		VectorExpression args11;
		args11.emplace_back(List);
		REQUIRE_THROWS_AS(realnumber(args11), SemanticError);

	INFO("real of 2 complex numbers")
		VectorExpression args11_1;
		args11_1.emplace_back((Expression(Atom(ComplexNumber(2, 3)))));
		args11_1.emplace_back((Expression(Atom(ComplexNumber(4, 5)))));
		REQUIRE_THROWS_AS(realnumber(args11_1), SemanticError);

	INFO("real of a complex number")
		VectorExpression args11_2;
		args11_2.emplace_back((Expression(Atom(ComplexNumber(2, 3)))));
		REQUIRE(realnumber(args11_2) == Expression(Atom(2)));

	INFO("imaginary of a non complex number")
		VectorExpression args12;
		args12.emplace_back(List);
		REQUIRE_THROWS_AS(imagnumber(args12), SemanticError);

	INFO("imaginary of 2 complex numbers")
		VectorExpression args12_1;
		args12_1.emplace_back((Expression(Atom(ComplexNumber(2, 3)))));
		args12_1.emplace_back((Expression(Atom(ComplexNumber(4, 5)))));
		REQUIRE_THROWS_AS(imagnumber(args12_1), SemanticError);

	INFO("imaginary of a complex number")
		VectorExpression args12_2;
		args12_2.emplace_back((Expression(Atom(ComplexNumber(2, 3)))));
		REQUIRE(imagnumber(args12_2) == Expression(Atom(3)));

	INFO("magnitude of a non complex number")
		VectorExpression args13;
		args13.emplace_back(List);
		REQUIRE_THROWS_AS(magnitude(args13), SemanticError);

	INFO("magnitude of 2 complex numbers")
		VectorExpression args13_1;
		args13_1.emplace_back((Expression(Atom(ComplexNumber(2, 3)))));
		args13_1.emplace_back((Expression(Atom(ComplexNumber(4, 5)))));
		REQUIRE_THROWS_AS(magnitude(args13_1), SemanticError);

	INFO("magnitude of a complex number")
		VectorExpression args13_2;
		args13_2.emplace_back((Expression(Atom(ComplexNumber(2, 3)))));
		REQUIRE(magnitude(args13_2) == magnitude(args13_2));

	INFO("argument of a non complex number")
		VectorExpression args14;
		args14.emplace_back(List);
		REQUIRE_THROWS_AS(argument(args14), SemanticError);

	INFO("argument of 2 complex numbers")
		VectorExpression args14_1;
		args14_1.emplace_back((Expression(Atom(ComplexNumber(2, 3)))));
		args14_1.emplace_back((Expression(Atom(ComplexNumber(4, 5)))));
		REQUIRE_THROWS_AS(argument(args14_1), SemanticError);

	INFO("argument of a complex number")
		VectorExpression args14_2;
		args14_2.emplace_back((Expression(Atom(ComplexNumber(2, 3)))));
		REQUIRE(argument(args14_2) == argument(args14_2));

	INFO("conjugate of a non complex number")
		VectorExpression args15;
		args15.emplace_back(List);
		REQUIRE_THROWS_AS(conjugate(args15), SemanticError);

	INFO("conjugate of 2 complex numbers")
		VectorExpression args15_1;
		args15_1.emplace_back((Expression(Atom(ComplexNumber(2, 3)))));
		args15_1.emplace_back((Expression(Atom(ComplexNumber(4, 5)))));
		REQUIRE_THROWS_AS(conjugate(args15_1), SemanticError);

	INFO("conjugate of a complex number")
		VectorExpression args15_2;
		args15_2.emplace_back((Expression(Atom(ComplexNumber(2, 3)))));
		REQUIRE(conjugate(args15_2) == conjugate(args15_2));
}		

TEST_CASE("Test get all built-in procedure for list", "[environment]") {

	Environment env;

	Procedure makelist = env.get_proc(Atom("list"));
	Procedure firstinlist = env.get_proc(Atom("first"));
	Procedure restoflist = env.get_proc(Atom("rest"));
	Procedure listsize = env.get_proc(Atom("length"));
	Procedure appending = env.get_proc(Atom("append"));
	Procedure joinlist = env.get_proc(Atom("join"));
	Procedure rangelist = env.get_proc(Atom("range"));
	Procedure applyproc = env.get_proc(Atom("apply"));

	VectorExpression goodListElement;
	VectorExpression goodListElement1;
	VectorExpression emptyListElement;
	for (int i = 0; i < 5; i++)
		goodListElement.push_back(Atom(i));
	goodListElement.push_back(Atom(ComplexNumber(2, 3)));

	for (int i = 5; i < 10; i++)
		goodListElement1.push_back(Atom(i));

	Expression expGoodList = makelist(goodListElement);
	Expression expGoodList1 = makelist(goodListElement1);
	Expression expEmptyList = makelist(emptyListElement);

	VectorExpression argsGoodList; argsGoodList.push_back(expGoodList);
	VectorExpression argsGoodList1; argsGoodList1.push_back(expGoodList1);
	VectorExpression argsEmptyList; argsEmptyList.push_back(expEmptyList);
	VectorExpression args2ListsGood; args2ListsGood.push_back(expGoodList);
	args2ListsGood.push_back(expGoodList1);
	VectorExpression argsNotList; argsNotList.push_back(Expression(Atom("lambda")));

	VectorExpression args2ListsNotGood; args2ListsNotGood.push_back(Expression(Atom("lambda")));
	args2ListsNotGood.push_back(expGoodList);

	INFO("Test first in list")
	REQUIRE(firstinlist(argsGoodList) == Expression(0));
	REQUIRE_THROWS_AS(firstinlist(argsEmptyList), SemanticError);
	REQUIRE_THROWS_AS(firstinlist(argsNotList), SemanticError);
	REQUIRE_THROWS_AS(firstinlist(args2ListsGood), SemanticError);

	INFO("Test rest of list")
		VectorExpression compareVar;
	for (int i = 1; i < 5; i++)
		compareVar.push_back(Atom(i));
	compareVar.push_back(Atom(ComplexNumber(2, 3)));
	Expression expCompareVar = makelist(compareVar);

	REQUIRE(restoflist(argsGoodList) == expCompareVar);
	REQUIRE_THROWS_AS(restoflist(argsEmptyList), SemanticError);
	REQUIRE_THROWS_AS(restoflist(argsNotList), SemanticError);
	REQUIRE_THROWS_AS(restoflist(args2ListsGood), SemanticError);

	INFO("Test list size")
	REQUIRE(listsize(argsGoodList) == Expression(Atom(6)));
	REQUIRE(listsize(argsEmptyList) == Expression(Atom(0)));
	REQUIRE_THROWS_AS(listsize(args2ListsGood), SemanticError);
	REQUIRE_THROWS_AS(listsize(argsNotList), SemanticError);

	INFO("Test appending")
		VectorExpression compareVarAppen;
	for (auto a : goodListElement)
		compareVarAppen.push_back(a);
	for (auto a : goodListElement1)
		compareVarAppen.push_back(a);
	Expression expAppendList = makelist(compareVarAppen);

	REQUIRE_THROWS_AS(appending(argsGoodList), SemanticError);
	REQUIRE_THROWS_AS(appending(argsNotList), SemanticError);
	REQUIRE_THROWS_AS(appending(args2ListsNotGood), SemanticError);
	REQUIRE(appending(args2ListsGood) == appending(args2ListsGood));

	INFO ("Test join list")
	REQUIRE_THROWS_AS(joinlist(argsGoodList), SemanticError);
	REQUIRE_THROWS_AS(joinlist(argsNotList), SemanticError);
	REQUIRE_THROWS_AS(joinlist(args2ListsNotGood), SemanticError);
	REQUIRE(joinlist(args2ListsGood) == expAppendList);

	INFO ("Test range list")
		VectorExpression rangeVectorGood;
		VectorExpression rangeVectorBad1;
		VectorExpression rangeVectorBad2;
		VectorExpression rangeVectorBad3;
	rangeVectorGood.push_back(Expression(Atom(0)));
	rangeVectorGood.push_back(Expression(Atom(5)));
	rangeVectorGood.push_back(Expression(Atom(1)));

	rangeVectorBad1.push_back(Expression(Atom(3)));
	rangeVectorBad1.push_back(Expression(Atom(-1)));
	rangeVectorBad1.push_back(Expression(Atom(1)));

	rangeVectorBad2.push_back(Expression(Atom(0)));
	rangeVectorBad2.push_back(Expression(Atom(5)));
	rangeVectorBad2.push_back(Expression(Atom(-1)));

	rangeVectorBad3.push_back(Expression(Atom(0)));
	rangeVectorBad3.push_back(Expression(Atom(5)));
	rangeVectorBad3.push_back(Expression(Atom("list")));

	REQUIRE_THROWS_AS(rangelist(rangeVectorBad1), SemanticError);
	REQUIRE_THROWS_AS(rangelist(rangeVectorBad2), SemanticError);
	REQUIRE_THROWS_AS(rangelist(argsEmptyList), SemanticError);
	REQUIRE_THROWS_AS(rangelist(rangeVectorBad3), SemanticError);
	REQUIRE(rangelist(rangeVectorGood) == rangelist(rangeVectorGood));

}
