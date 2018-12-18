/*! \file atom.hpp
Defines the Atom type and associated functions.
 */
#ifndef ATOM_HPP
#define ATOM_HPP

#include "token.hpp"

/*! \class Atom
\brief A variant type that may be a Number or Symbol or the default type None.

This class provides value semantics.
*/
class Atom {
public:

  /// Construct a default Atom of type None
  Atom();

  /// Construct an Atom of type Complex Number with value
  Atom(ComplexNumber value);

  /// Construct an Atom of type Number with value
  Atom(double value);

  /// Construct an Atom of type Symbol named value
  Atom(const std::string & value);

  /// Construct an Atom directly from a Token
  Atom(const Token & token);

  /// Copy-construct an Atom
  Atom(const Atom & x);

  /// Assign an Atom
  Atom & operator=(const Atom & x);

  /// Atom destructor
  ~Atom();

  /// predicate to determine if an Atom is of type None
  bool isNone() const noexcept;

  /// predicate to determine if an Atom is of type Number
  bool isNumber() const  noexcept;

  /// predicate to determine if an Atom is of type Symbol
  bool isSymbol() const noexcept;

  /// predicate to determine if an Atom is a complex number
  bool isComplexNumber() const noexcept;

  /// predicate to determine if an Atom is a string type
  bool isStringConstant() const noexcept;

  /// value of Atom as a number, return 0 if not a Number
  double asNumber() const noexcept;

  /// value of Atom as a symbol, returns empty-string if not a Symbol
  std::string asSymbol() const noexcept;

  /// value of Atom as a string, return empty-string if not a string
  std::string asStringConstant() const noexcept;

  /// value of Atom as a complex number, return (0,0) if its not
  ComplexNumber asComplexNumber() const noexcept;

  /// equality comparison based on type and value
  bool operator==(const Atom & right) const noexcept;

  /// get value of real number of complex number, return 0.0 if its not
  double asRealNumber() const noexcept;

  /// get value of real number of complex number, return 0.0 if its not
  double asImaginaryNumber() const noexcept;
  
  // set checker of list is inside lambda
  void setInsideLambda();

  // check to see if the list inside lambda
  bool isInsideLambda() const noexcept;

  ///
  void setStringType();

private:

  // internal enum of known types
  enum Type {NoneKind, NumberKind, SymbolKind, ComplexKind, StringKind};

  // track the type
  Type m_type;

  // values for the known types. Note the use of a union requires care
  // when setting non POD values (see setSymbol)
  union {
    double numberValue;
    std::string stringValue;
	std::complex <double> complexValue;
  };
  bool insideLambda;
  // helper to set type and value of Number
  void setNumber(double value);

  // helper to set type and value of Symbol
  void setSymbol(const std::string & value);

  // helper to set type and value of ComplexNumber
  void setComplex(const ComplexNumber & value);

  // helper to set type and value for a "String"
  void setStringConstant(const std::string & value);
};

/// inequality comparison for Atom
bool operator!=(const Atom &left, const Atom & right) noexcept;

/// output stream rendering
std::ostream & operator<<(std::ostream & out, const Atom & a);

#endif
