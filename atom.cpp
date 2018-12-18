#include "atom.hpp"
#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>


Atom::Atom()
{
	m_type = NoneKind;
	insideLambda = false;
}

Atom::Atom(ComplexNumber value)
{
	setComplex(value);
}

Atom::Atom(double value){
  setNumber(value);
}

Atom::Atom(const std::string & value) : Atom() {
	setSymbol(value);
}

Atom::Atom(const Token & token): Atom(){
  
  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  // you only can assign istringstream to a double or in by using >> operator
  // if its able to assign, the operator will return true.
  if(iss >> temp){
    // check for trailing characters if >> succeeds
	  // what does it mean ? doesnt token always have either number or symbol ?
    if(iss.rdbuf()->in_avail() == 0){
      setNumber(temp);
    }
  }
  else{ // else assume symbol
    // make sure does not start with number
    if(!std::isdigit(token.asString()[0])){
      setSymbol(token.asString());
    }
  }
}

Atom::Atom(const Atom & x): Atom(){
  if(x.isNumber()){
    setNumber(x.numberValue);
  }
  else if(x.isSymbol()){
    setSymbol(x.stringValue);
  }
  else if (x.isStringConstant()) {
	  setStringConstant(x.stringValue);
  }
}

Atom & Atom::operator=(const Atom & x){

  if(this != &x){
    if(x.m_type == NoneKind){
      m_type = NoneKind;
    }
    else if(x.m_type == NumberKind){
      setNumber(x.numberValue);
    }
    else if(x.m_type == SymbolKind){
      setSymbol(x.stringValue);
    }
	else if (x.m_type == ComplexKind) {
		setComplex(x.complexValue);
	}
	else if (x.m_type == StringKind) {
		setStringConstant(x.stringValue);
	}
	insideLambda = x.insideLambda;
  }
  return *this;
}
  
Atom::~Atom(){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }
}

bool Atom::isNone() const noexcept{
  return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept{
  return m_type == NumberKind;
}

bool Atom::isSymbol() const noexcept{
  return m_type == SymbolKind;
}

bool Atom::isComplexNumber() const noexcept
{
	return m_type == ComplexKind;
}

bool Atom::isStringConstant() const noexcept
{
	return m_type == StringKind;
}


void Atom::setNumber(double value){

  m_type = NumberKind;
  numberValue = value;
}

void Atom::setSymbol(const std::string & value){

  // we need to ensure the destructor of the symbol string is called
	// why do we need to ensure that ? what would it do ?
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }
    
  m_type = SymbolKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}

void Atom::setComplex(const ComplexNumber& value)
{
	m_type = ComplexKind;
	complexValue = value;
}

void Atom::setStringConstant(const std::string & value)
{
	if (m_type == StringKind) {
		stringValue.~basic_string();
	}

	m_type = StringKind;

	// copy construct in place
	new (&stringValue) std::string(value);
}


double Atom::asNumber() const noexcept{

  return (m_type == NumberKind) ? numberValue : 0.0;  
}


std::string Atom::asSymbol() const noexcept{

  std::string result;

  if(m_type == SymbolKind){
    result = stringValue;
  }

  return result;
}

std::string Atom::asStringConstant() const noexcept
{
	std::string result;
	if (m_type == StringKind)
		result = stringValue;

	return result;
}

ComplexNumber Atom::asComplexNumber() const noexcept
{
	ComplexNumber returnVariable(0, 0);
	if (m_type == ComplexKind)
		returnVariable = complexValue;
	return returnVariable;
}

bool Atom::operator==(const Atom & right) const noexcept{
  
  if(m_type != right.m_type) return false;
  // compare the kind of atom first then compare the value
  switch(m_type){
  case NoneKind:
    if(right.m_type != NoneKind) return false;
    break;
  case NumberKind:
    {
      if(right.m_type != NumberKind) return false;
      double dleft = numberValue;
      double dright = right.numberValue;
	  // abs only works with interger, fabs works works for floating point value
      double diff = fabs(dleft - dright);
	  // isnan will be true if its not a zero.
      if(diff > 0.0000001) return false;
    }
    break;
  case SymbolKind:
    {
      if(right.m_type != SymbolKind) return false;

      return stringValue == right.stringValue;
    }
    break;
  case ComplexKind:
    {
	  if (right.m_type != ComplexKind) return false;
	  
	  double diff1 = fabs(this->asRealNumber() - right.asRealNumber());
	  double diff2 = fabs(this->asImaginaryNumber() - right.asImaginaryNumber());

	  if ((diff1 > 0.0000001) || (diff2 > 0.0000001)) return false;
    }
	break;
  case StringKind:
  {
	  if (right.m_type != StringKind) return false;

	  return stringValue == right.stringValue;
  }
    break;
  default:
    return false;
  }

  return true;
}

double Atom::asRealNumber() const noexcept
{
	return (m_type == ComplexKind) ? std::real(complexValue) : 0.0;
}

double Atom::asImaginaryNumber() const noexcept
{
	return (m_type == ComplexKind) ? std::imag(complexValue) : 0.0;
}

void Atom::setInsideLambda()
{
	insideLambda = true;
}

bool Atom::isInsideLambda() const noexcept
{
	return insideLambda;
}

void Atom::setStringType()
{
	m_type = StringKind;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{
  
  return !(left == right);
}


std::ostream & operator<<(std::ostream & out, const Atom & a){

  if(a.isNumber()){
    out << a.asNumber();
  }
  if(a.isSymbol()){
    out << a.asSymbol();
  }
  if (a.isComplexNumber())
  {
	  out << a.asRealNumber() << "," <<  a.asImaginaryNumber();
  }
  if (a.isStringConstant())
  {
	  out << '"' << a.asStringConstant() << '"';
  }
  return out;
}
