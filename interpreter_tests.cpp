#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
  
  std::istringstream iss(program);
    
  Interpreter interp;
    
  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl; 
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);
 
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};
  
  for(auto program : programs){
    std::istringstream iss(program);
 
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");
    
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);
  
    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
  
  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {
  
  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }
  
  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}
  
TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }
  
  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)"};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {
  
  std::vector<std::string> programs = {"(@ none)", // so such procedure
				       "(- 1 1 2)", // too many arguments
				       "(define begin 1)", // redefine special form
				       "(define pi 3.14)"}; // redefine builtin symbol
    for(auto s : programs){
      Interpreter interp;

      std::istringstream iss(s);
      
      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);
      
    }
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
(1 2 3)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test lambda with good input", "[interpreter]") {
	std::string input = R"((lambda (x) (* 2 x)))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	Expression result = run(input);
	REQUIRE(result == run(input));
}

TEST_CASE("Test lambda with invalid number of argument", "[interpreter]") {
	std::string input = R"((lambda (x) (* 2 x) (2)))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test lambda with first argument is not a symbol", "[interpreter]") {
	std::string input = R"((lambda (2) (* 2 x)))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test lambda with first argument is a build in procedure", "[interpreter]") {
	std::string input = "(lambda (+) (* 2 x))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test lambda with first argument is a special form", "[interpreter]") {
	std::string input = "(lambda (define) (2))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test apply with good input", "[interpreter]") {
	std::string input = "(apply + (list 1 2 3 4))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	Expression result = run(input);
	REQUIRE(result == run(input));
}

TEST_CASE("Test apply with bad input", "[interpreter]") {
	std::vector<std::string> input = {  "(apply (+ z I) (list 1 2 3))",
										"(apply + (list 1 2) 3)",
										"(apply + 3)"};

	Interpreter interp;
	for (auto a : input)
	{
		std::istringstream iss(a);

		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}
}

TEST_CASE("Test apply with a user define input", "[interpreter]") {
	std::vector<std::string> input = { "(define linear(lambda(a b x) (+(* a x) b)))",
										"(apply linear (list 1 2 3))"};
									

	Interpreter interp;
	for (auto a : input)
	{
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
}


TEST_CASE("Test map with good input", "[interpreter]") {
	std::vector<std::string> input = { "(define f (lambda (x) (sin x)))",
										"(map f (list (- pi) (/ (- pi) 2) 0 (/ pi 2) pi))" };


	Interpreter interp;
	for (auto a : input)
	{
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
}

TEST_CASE("Test map with bad input", "[interpreter]") {
	std::vector<std::string> input = { "(map + (list 1 2 3) (list 2 3))",
										"(map x (list 1 2 3))",
										"(map + 3)", 
										"(map (list 1 2 3) (list 1 2))"};

	Interpreter interp;
	for (auto a : input)
	{
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}
}

TEST_CASE("Test define with bad input", "[interpreter]") {
	std::vector<std::string> input = { "(define 2 3)",
										"(define define 2)",
										"(define + 4)" };

	Interpreter interp;
	for (auto a : input)
	{
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}
}


TEST_CASE("Test making string with good string type and string number", "[interpreter]") {
	std::vector<std::string> input = {  ("(\"foo\")"), 
										("(\"hello world\")"),
										("(\"123456\")")};

	Interpreter interp;
	for (auto a : input)
	{
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		Expression result = run(a);
		REQUIRE(result.isHeadStringConstant() == true);
		REQUIRE(result.isHeadSymbol() == false);
	}
}

TEST_CASE("Test making string with bad string type", "[interpreter]") {
	std::vector<std::string> input = { ("(\"foo)"),
										("(\"hello \" world\")"),
										("(\"123456\" \")") };

	Interpreter interp;
	for (auto a : input)
	{
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == false);
	}
}


TEST_CASE("Test set property and get property", "[interpreter]") {
	std::vector<std::string> input = { "(define pet (set-property \"color\" \"black\" \"cat\"))" ,
										"(get-property \"color\" pet)"};

	Interpreter interp;

	for (auto a : input)
	{
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
}

TEST_CASE("Test set bad property and get bad property", "[interpreter]") {
	std::vector<std::string> input = { "(define pet (set-property \"color\" \"black\" \"cat\" 3))" ,
										"(define + (set-property \"color\" \"black\" \"cat\"))",
										"(define + (set-property 4 \"black\" \"cat\"))",
										"(get-property \"nothing\" pet)",
										"(get-property \"color\" pet 3)",
										"(get-property 3 pet)", };

	Interpreter interp;

	for (auto a : input)
	{
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
	}

	Atom a("test"); a.setStringType();
	Atom b("test1"); b.setStringType();

	REQUIRE(a == a);
	REQUIRE(a != b);
}

TEST_CASE("Test discrete plot", "[interpreter]") {
	std::vector<std::string> input = { " (define f (lambda (x) (list x(+ (* 2 x) 1))))",
										"(discrete-plot (map f (range -2 2 0.5))"
										"(list (list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\")"
										"(list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1)))"};
	
	Interpreter interp;

	for (auto a : input)
	{
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_NOTHROW(interp.evaluate());
	}
}


TEST_CASE("Test continuous plot", "[interpreter]") {
	std::vector<std::string> input = { " (define f (lambda (x) (+(* 2 x) 1)))",
										"(continuous-plot f (list -2 2)	(list (list \"title\" \"A continuous linear function\") (list \"abscissa-label\" \"x\")	(list \"ordinate-label\" \"y\")))" };

	Interpreter interp;

	for (auto a : input)
	{
		std::istringstream iss(a);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		REQUIRE_NOTHROW(interp.evaluate());
	}
}

typedef message_queue<std::string> MessageQueueStr;

TEST_CASE("Test Interrupt", "[interpreter]") {
	std::vector<std::string> input = { " (define f (lambda (x) (list x(+ (* 2 x) 1))))",
										"(discrete-plot (map f (range -2 2 0.5))"
										"(list (list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\")"
										"(list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1)))" };

	Interpreter interp;
	MessageQueueStr intrruptSignal;
	interp.setInterrupSig(&intrruptSignal);
}
