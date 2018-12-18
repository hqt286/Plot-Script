#include "token.hpp"

// system includes
#include <cctype>
#include <iostream>

// define constants for special characters
const char OPENCHAR = '(';
const char CLOSECHAR = ')';
const char COMMENTCHAR = ';';
const char QUOTATIONCHAR = '"';

Token::Token(TokenType t): m_type(t){}

Token::Token(const std::string & str): m_type(STRING), value(str) {}

Token::TokenType Token::type() const{
  return m_type;
}

std::string Token::asString() const{
  switch(m_type){
  case OPEN:
    return "(";
  case CLOSE:
    return ")";
  case STRING:
    return value;
  case OPENQUOTATION:
	  return "\"";
  case CLOSEQUOTATION:
	  return "\"";
  }
  return "";
}


// add token to sequence unless it is empty, clears token
void store_ifnot_empty(std::string & token, TokenSequenceType & seq){
  if(!token.empty()){
	  // .emplace_back() is similar to push back. But emplace and put thing on top or back. More advance for string for vector
    seq.emplace_back(token);
    token.clear();
  }
}

TokenSequenceType tokenize(std::istream & seq){
  TokenSequenceType tokens;
  std::string token;
  bool openquotation = false;
  while (true) {
	  char c = seq.get();
	  if (seq.eof()) break;

	  if (openquotation == true && c == QUOTATIONCHAR)
	  {
		  store_ifnot_empty(token, tokens);
		  tokens.push_back(Token::TokenType::CLOSEQUOTATION);
		  openquotation = false;
	  }

	  else if (openquotation == true)
	  {
		  token.push_back(c);
	  }

	  else
	  {
		  if (c == COMMENTCHAR) {
			  // chomp until the end of the line
			  while ((!seq.eof()) && (c != '\n')) {
				  c = seq.get();
			  }
			  if (seq.eof()) break;
		  }
		  else if (c == OPENCHAR) {
			  store_ifnot_empty(token, tokens);
			  tokens.push_back(Token::TokenType::OPEN);
		  }
		  else if (c == CLOSECHAR) {
			  store_ifnot_empty(token, tokens);
			  tokens.push_back(Token::TokenType::CLOSE);
		  }

		  else if (c == QUOTATIONCHAR) {
			  store_ifnot_empty(token, tokens);
			  if (openquotation == false) {
				  tokens.push_back(Token::TokenType::OPENQUOTATION);
				  openquotation = true;
			  }
		  }
		  // ispace is to check if there is a whitespace character
		  // if there is a space, clear out that space
		  else if (isspace(c)) {
			  store_ifnot_empty(token, tokens);
		  }
		  else {
			  token.push_back(c);
		  }
	  }
  }
  store_ifnot_empty(token, tokens);

  return tokens;
}
// emplace vs push back
//vector <foo> name. assume full is a class. 
// emplace back will automaticly create foo then pass in argument inside foo (short cut)