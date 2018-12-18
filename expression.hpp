/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include<algorithm>
#include <iomanip>

#include "token.hpp"
#include "atom.hpp"
#include "message_queue.h"


const int BOXSCALE = 20;
const double POINTSIZE = 0.5;
const int LINETHICKNESS = 0;
const int OUTTERLINE = 3;
const int SAMPLING = 50;
typedef message_queue<std::string> MessageQueueStr;

// forward declare Environment
class Environment;

/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty) 
list of expressions called the tail.
 */
class Expression {
public:

  typedef std::vector<Expression>::const_iterator ConstIteratorType;

  /// Default construct and Expression, whose type in NoneType
  Expression();

  /*! Construct an Expression with given Atom as head an empty tail
    \param atom the atom to make the head
  */
  Expression(const Atom & a);

  /// deep-copy construct an expression (recursive)
  Expression(const Expression & a);

  /// deep-copy assign an expression  (recursive)
  Expression & operator=(const Expression & a);

  /// return a reference to the head Atom
  Atom & head();

  /// return a const-reference to the head Atom
  const Atom & head() const;

  /// append Atom to tail of the expression
  void append(const Atom & a);

  /// push back an expression to tail of expression vector
  void pushback(const Expression & a);

  /// return a pointer to the last expression in the tail, or nullptr
  Expression * tail();

  /// return a point to the first expression in the tail
  Expression * first_of_tail();

  /// set property into the expression
  void add_property(const std::string & keyword,const Expression & exp);

  // return size of tail
  int tailSize() const noexcept;

  int propertySize() const noexcept;

  //get expression property inside property map
  Expression getProperty(const std::string & keyword) const noexcept;

  // replace the variable inside lambda with the input variable
  //Expression replace_LambdaVariables(const Expression & argument, const Expression & procedure);

  /// return a const-iterator to the beginning of tail
  ConstIteratorType tailConstBegin() const noexcept;

  /// return a const-iterator to the tail end
  ConstIteratorType tailConstEnd() const noexcept;

  /// convienience member to determine if head atom is a number
  bool isHeadNumber() const noexcept;

  /// convienience member to determine if head atom is a string constant
  bool isHeadStringConstant() const noexcept;

  /// convienience member to determine if head atom is a symbol
  bool isHeadSymbol() const noexcept;
   
  /// convienience member to determine if head atom is a complextype
  bool isHeadComplex() const noexcept;

  /// check to see if tail is empty
  bool isTailEmpty() const noexcept;

  /// Evaluate expression using a post-order traversal (recursive)
  Expression eval(Environment & env);

  /// equality comparison for two expressions (recursive)
  bool operator==(const Expression & exp) const noexcept;

private:

  // the head of the expression
  Atom m_head;

  // the tail list is expressed as a vector for access efficiency
  // and cache coherence, at the cost of wasted memory.
  std::vector<Expression> m_tail;

  // convenience typedef
  typedef std::vector<Expression>::iterator IteratorType;
  
  std::map <std::string, Expression> m_property;

  // internal helper methods
  Expression handle_lookup(const Atom & head, const Environment & env);
  Expression handle_define(Environment & env);
  Expression handle_begin(Environment & env);
  Expression handle_lambda(Environment & env);
  Expression handle_apply(Environment & env);
  Expression handle_map(Environment & env);
  Expression handle_setprop(Environment & env);
  Expression handle_getprop(Environment & env);
  Expression handle_continuousplot(Environment & env);
};
/// 


/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;
  
#endif

/*********************** helper function *************************/
/******************************************************************/
Expression makePoint(double x, double y, double point_size);
Expression makeLine(Expression point1, Expression point2);
void get_borderLine(std::unordered_map<std::string, double> data, Expression & result);
std::unordered_map<std::string, double> checkAndScalePoints(Expression points);
double getTextScale(Expression options);
double getTextScale(Expression options);
void checkAndMakeOptionList(Expression option, Expression & result, double text_scale, std::unordered_map<std::string, double> data);
void addAlAuOlOu(std::unordered_map<std::string, double> data, Expression & result, double text_scale);
