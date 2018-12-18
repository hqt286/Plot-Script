#include "parse.hpp"

#include <stack>

bool setHead(Expression &exp, const Token &token, bool isstring) {

	Atom a;
	if (isstring)
	{
		a = Atom(token.asString());
		a.setStringType();
	}
	else
		a = Atom(token);

	exp.head() = a;

	return !a.isNone();
}

bool append(Expression *exp, const Token &token, bool isstring) {

	Atom a;
	if (isstring)
	{
		a = Atom(token.asString());
		a.setStringType();
	}
	else
		a = Atom(token);

	exp->append(a);

	return !a.isNone();
}

Expression parse(const TokenSequenceType &tokens) noexcept {

	Expression ast;

	// cannot parse empty
	if (tokens.empty())
		return Expression();

	bool athead = false;
	bool instringconstant = false;
	// stack tracks the last node created
	std::stack<Expression *> stack;

	std::size_t num_tokens_seen = 0;

	for (auto &t : tokens) {

		if (t.type() == Token::OPENQUOTATION) {
			instringconstant = true;
		}

		else if (t.type() == Token::CLOSEQUOTATION) {
			instringconstant = false;
		}

		else if (t.type() == Token::OPEN) {
			athead = true;
		}
		else if (t.type() == Token::CLOSE) {
			if (stack.empty()) {
				return Expression();
			}
			stack.pop();

			if (stack.empty()) {
				num_tokens_seen += 1;
				break;
			}
		}
		else {

			if (athead) {
				if (stack.empty()) {
					if (!setHead(ast, t, instringconstant)) {
						return Expression();
					}
					stack.push(&ast);
				}
				else {
					if (stack.empty()) {
						return Expression();
					}
					// if adding a nontype is true
					if (!append(stack.top(), t, instringconstant)) {
						return Expression();
					}
					stack.push(stack.top()->tail());
				}
				athead = false;
			}
			else {
				if (stack.empty()) {
					return Expression();
				}

				if (!append(stack.top(), t, instringconstant)) {
					return Expression();
				}
			}
		}
		num_tokens_seen += 1;
	}
	// open will make it push into stack and close will make it pop out of stact
	if (stack.empty() && (num_tokens_seen == tokens.size())) {
		return ast;
	}

	return Expression();
};
