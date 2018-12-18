#include "interpreter.hpp"

// system includes
#include <stdexcept>

// module includes
#include "token.hpp"
#include "parse.hpp"
#include "expression.hpp"
#include "environment.hpp"
#include "semantic_error.hpp"

bool Interpreter::parseStream(std::istream & expression) noexcept{

  TokenSequenceType tokens = tokenize(expression);
  // this will be now tokens = OPEN + 2 3 CLOSE. Each string is stored inside a single tokent
  ast = parse(tokens);

  return (ast != Expression());
};
				     

Expression Interpreter::evaluate(){

  return ast.eval(env);
}

void Interpreter::setInterrupSig(MessageQueueStr * signal)
{
	env.setInterruptSignal(signal);
}
