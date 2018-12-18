/*! \file interpreter.hpp

The interpreter can parse from a stream into an internal AST and evaluate it.
It maintains an environment during evaluation.
 */

#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

// system includes
#include <istream>
#include <string>

// module includes
#include "environment.hpp"
#include "expression.hpp"

/*! \class Interpreter
\brief Class to parse and evaluate an expression (program)

Interpreter has an Environment, which starts at a default.
The parse method builds an internal AST.
The eval method updates Environment and returns last result.
*/
class Interpreter {
public:

  /*! Parse into an internal Expression from a stream
    \param expression the raw text stream repreenting the candidate expression
    \return true on successful parsing 
   */
  bool parseStream(std::istream &expression) noexcept;

  /*! Evaluate the Expression by walking the tree, returning the result.
    \return the Expression resulting from the evaluation in the current environment
    \throws SemanticError when a semantic error is encountered
   */
  Expression evaluate();

  void setInterrupSig(MessageQueueStr * signal);

private:

  // the environment
  Environment env;

  // the AST
  Expression ast;
};

#endif
