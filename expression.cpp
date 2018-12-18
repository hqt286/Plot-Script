#include "expression.hpp"

#include <sstream>
#include <list>
#include<algorithm>
#include "environment.hpp"
#include "semantic_error.hpp"


/************************************************************************************************************************************
Helper Functions
**************************************************************************************************************************************/

Expression calcLambda(Atom lambda_name, Expression input, Environment env)
{
	Expression process(lambda_name);
	process.pushback(input);
	return process.eval(env);
}

//smooth out the vector
void curveVector(Expression pointA, Expression pointB, Expression pointC, std::vector<Expression> & result, Atom lambda_name, Environment env)
{

	double A_x = pointA.first_of_tail()->head().asNumber();
	double A_y = pointA.tail()->head().asNumber();
	double B_x = pointB.first_of_tail()->head().asNumber();
	double B_y = pointB.tail()->head().asNumber();
	double C_x = pointC.first_of_tail()->head().asNumber();
	double C_y = pointC.tail()->head().asNumber();

	//get magnitutde of each vector and calculate the angle
	double vectorAB = std::pow(B_x - A_x, 2) + std::pow(B_y - A_y, 2);
	double vectorBC = std::pow(B_x - C_x, 2) + std::pow(B_y - C_y, 2);
	double vectorAC = std::pow(C_x - A_x, 2) + std::pow(C_y - A_y, 2);
	double angle = std::acos((vectorAB + vectorBC - vectorAC) / std::sqrt(4 * vectorAB * vectorBC));
	double angleDegree = 180 * angle / std::atan2(0, -1);

	//get mid x for A and B
	double midAB_x = (A_x + B_x) / 2;
	double midBC_x = (B_x + C_x) / 2;

	// calculate mid y for AB and BC
	Expression temp_midAB_y = calcLambda(lambda_name, Expression(midAB_x), env);
	Expression temp_midBC_y = calcLambda(lambda_name, Expression(midBC_x), env);
	double midAB_y = temp_midAB_y.head().asNumber();
	double midBC_y = temp_midBC_y.head().asNumber();

	// make mid AB point and mid BC point
	Expression midAB = makePoint(midAB_x, midAB_y, 0);
	Expression midBC = makePoint(midBC_x, midBC_y, 0);

	if (angleDegree < 175)
	{
		result.push_back(pointA);
		result.push_back(midAB);
		result.push_back(pointB);
		result.push_back(midBC);
	}
	else
	{
		result.push_back(pointA);
		result.push_back(pointB);
	}
	return;
}
/************************************************************************************************************************************
END
**************************************************************************************************************************************/


Expression::Expression() {}

Expression::Expression(const Atom & a) {

	m_head = a;
}

// recursive copy
Expression::Expression(const Expression & a) {

	m_head = a.m_head;
	m_property = a.m_property;
	for (auto e : a.m_tail) {
		m_tail.push_back(e);
	}
}

Expression & Expression::operator=(const Expression & a) {

	// prevent self-assignment
	// compare 2 mem address, not the object inside it.
	if (this != &a) {
		m_head = a.m_head;
		m_property = a.m_property;
		m_tail.clear();
		for (auto e : a.m_tail) {
			m_tail.push_back(e);
		}
	}

	return *this;
}

// why one with constant atom ? 
Atom & Expression::head() {
	return m_head;
}

const Atom & Expression::head() const {
	return m_head;
}

bool Expression::isHeadNumber() const noexcept {
	return m_head.isNumber();
}

bool Expression::isHeadStringConstant() const noexcept
{
	return m_head.isStringConstant();
}

bool Expression::isHeadSymbol() const noexcept {
	return m_head.isSymbol();
}

bool Expression::isHeadComplex() const noexcept
{
	return m_head.isComplexNumber();
}

bool Expression::isTailEmpty() const noexcept
{
	return m_tail.empty();
}


void Expression::append(const Atom & a) {
	m_tail.emplace_back(a);
}

void Expression::pushback(const Expression & a) {
	m_tail.push_back(a);
}

Expression * Expression::tail() {
	Expression * ptr = nullptr;

	if (m_tail.size() > 0) {
		ptr = &m_tail.back();
	}

	return ptr;
}

Expression * Expression::first_of_tail()
{
	Expression * ptr = nullptr;

	if (m_tail.size() > 0) {
		ptr = &m_tail.front();
	}

	return ptr;
}

 
int Expression::propertySize() const noexcept
{
	return m_property.size();
}

void Expression::add_property(const std::string & keyword, const Expression & exp)
{
	m_property[keyword] = exp;
}

int Expression::tailSize() const noexcept
{
	return m_tail.size();
}

Expression Expression::getProperty(const std::string & keyword) const noexcept
{
	Expression result;

	auto it = m_property.find(keyword);
	if (it != m_property.cend())
		result = it->second;

	return result;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept {
	return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept {
	return m_tail.cend();
}

// userDefineProc is a lambda tree and args is input argument from user
Expression handle_userDefine(Expression userDefineProc, const std::vector<Expression> & args, const Environment & env)
{
	Environment tempEnvironment = env;
	int argumentSize = args.size();
	if (!(userDefineProc.tailConstBegin()->tailSize() == argumentSize))
		throw SemanticError("Error during evaluation: unknown symbol");
	else
	{
		int index = 0;
		for (auto a = userDefineProc.tailConstBegin()->tailConstBegin(); a != userDefineProc.tailConstBegin()->tailConstEnd(); a++)
		{
			Expression Temp(Atom("define"));
			Temp.pushback(*a);
			Temp.pushback(args[index]);
			Temp.eval(tempEnvironment);
			index++;
		}
	}

	return userDefineProc.tail()->eval(tempEnvironment);;
}


Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env) {

	// head must be a symbol
	if (!op.isSymbol()) {
		throw SemanticError("Error during evaluation in apply: procedure name not symbol");
	}

	// must map to a proc
	if (!env.is_proc(op) && !env.is_userDefine(op)) {
		throw SemanticError("Error during evaluation in apply: symbol does not name a procedure");
	}

	Expression returnExpression;

	if (env.is_userDefine(op))
	{
		Expression userDefineProc = env.get_UserDefineProc(op);
		returnExpression = handle_userDefine(userDefineProc, args, env);
	}
	else
	{
		// map from symbol to proc
		Procedure proc = env.get_proc(op);
		// call proc with args
		returnExpression = proc(args);
	}

	return returnExpression;
}


Expression Expression::handle_lookup(const Atom & head, const Environment & env) {
	if (head.isSymbol()) { // if symbol is in env return value
		if (env.is_exp(head)) {
			return env.get_exp(head);
		}
		else {
			throw SemanticError("Error during evaluation: unknown symbol");
		}
	}
	else if (head.isNumber() || head.isComplexNumber()) {
		return Expression(head);
	}
	else if (head.isStringConstant()) {
		return Expression(head);
	}
	else {
		throw SemanticError("Error during evaluation: Invalid type in terminal expression");
	}
}


Expression Expression::handle_begin(Environment & env) {

	if (m_tail.size() == 0) {
		throw SemanticError("Error during evaluation: zero arguments to begin");
	}

	// evaluate each arg from tail, return the last
	Expression result;
	for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
		result = it->eval(env);
	}

	return result;
}


Expression Expression::handle_lambda(Environment & env)
{
	// tail must have size 2 or error
	if (m_tail.size() != 2)
		throw SemanticError("Error during evaluation: invalid number of arguments to lambda");

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol())
		throw SemanticError("Error during evaluation: first argument in lambda not symbol");

	// make an list of all argument and also check if the argument of the function is valid
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin"))
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");

	if (env.is_proc(m_tail[0].head().asSymbol()))
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");

	Expression argumentList = env.make_list(m_tail[0]);
	Expression result(Atom("lambda"));
	result.pushback(argumentList);
	m_tail[1].head().setInsideLambda();
	result.pushback(m_tail[1]);

	return result;
}


Expression Expression::handle_apply(Environment & env)
{

	if (!(this->tailSize() == 2))
		throw SemanticError("Error during evaluation: invalid argument of apply");

	if (!m_tail[0].isTailEmpty())
		throw SemanticError("Error during apply: first argument to apply not a precedure");

	if (m_tail[1].head().asSymbol() != "list")
		throw SemanticError("Error during apply: second argument to apply not a list");

	std::vector<Expression> answer;
	Expression results = (m_tail[1].eval(env));

	for (auto a = results.tailConstBegin(); a != results.tailConstEnd(); a++)
		answer.push_back(*a);

	return apply(m_tail.begin()->head().asSymbol(), answer, env);
}


Expression Expression::handle_define(Environment & env) {

	// tail must have size 3 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin")) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}
	// eval tail[1]
	Expression result = m_tail[1].eval(env);

	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}
	
	//and add to env
	env.add_exp(m_tail[0].head(), result);

	return result;
}


Expression Expression::handle_map(Environment & env)
{
	Expression results = (m_tail[1].eval(env));

	if (!(this->tailSize() == 2))
		throw SemanticError("Error during evaluation: invalid argument of map");

	if (!m_tail[0].isTailEmpty())
		throw SemanticError("Error during map: first argument to map is not a precedure");

	if (results.head().asSymbol() != "list")
		throw SemanticError("Error during map: second argument to map not a list");

	Expression answerList(Atom("list"));

	for (auto a = results.tailConstBegin(); a != results.tailConstEnd(); a++)
	{
		std::vector<Expression> answer;
		answer.push_back(*a);
		answerList.pushback(apply(m_tail.begin()->head().asSymbol(), answer, env));
	}

	return answerList;
}


Expression Expression::handle_setprop(Environment & env)
{
	if (!(this->tailSize() == 3))
		throw SemanticError("Error in call to handle set property: invalid number of arguments.");

	if (!m_tail[0].isHeadStringConstant())
		throw SemanticError("Error : first argument to set-property is not a string.");

	Expression result;

	result = m_tail[2].eval(env);
	result.add_property(m_tail[0].head().asStringConstant(), m_tail[1].eval(env));

	return result;
}

Expression Expression::handle_getprop(Environment & env)
{
	if (!(this->tailSize() == 2))
		throw SemanticError("Error in handle get property: invalid number of arguments.");

	if (!m_tail[0].isHeadStringConstant())
		throw SemanticError("Error in handle get property: first argument is not a string.");

	Expression temp = m_tail[1].eval(env);

	return temp.getProperty(m_tail[0].head().asStringConstant());
}

Expression Expression::handle_continuousplot(Environment & env)
{
	Expression user_lambda = m_tail[0];
	Expression bounder_list = m_tail[1].eval(env);
	Expression option_list;

	double text_scale = 1;
	if (this->tailSize() > 2)
	{
		option_list = m_tail[2].eval(env);
		text_scale = getTextScale(option_list);
	}

	Expression result(Atom("list"));
	double xMax = bounder_list.tail()->head().asNumber();
	double xMin = bounder_list.first_of_tail()->head().asNumber();
	double each_sampling = (xMax - xMin) / SAMPLING;
	Expression point_list(Atom("list"));
	std::vector<Expression> point_list_vector;

	for (int i = 0; i <= SAMPLING; i++)
	{
		Expression process(Atom(user_lambda.head().asSymbol()));
		process.append(xMin);
		Expression answer = process.eval(env);
		Expression point = makePoint(xMin, answer.head().asNumber(), 0);
		point_list.pushback(point);
		point_list_vector.push_back(point);
		xMin = xMin + each_sampling;
	}


	std::vector<Expression> smoothData;
	std::vector<Expression> CopyData = point_list_vector;
	for (unsigned index = 0; index < 5; index++)
	{
		smoothData.clear();
		int SizeData = CopyData.size();
		for (int i = 0; i < SizeData - 2; i = i + 2)
		{
			curveVector(CopyData[i], CopyData[i + 1], CopyData[i + 2], smoothData, user_lambda.head(), env);
		}
		if ((smoothData.size() % 3) == 1)
		{
			smoothData.push_back(CopyData[SizeData - 2]);
			smoothData.push_back(CopyData.back());
		}
		else
		{
			smoothData.push_back(CopyData.back());
		}
		CopyData = smoothData;
	}


	Expression temp_point_list(Atom("list"));
	for (unsigned int each = 0; each < smoothData.size(); each++)
	{
		temp_point_list.pushback(smoothData[each]);
	}

	std::unordered_map<std::string, double> data_prop = checkAndScalePoints(temp_point_list);

	std::vector<Expression> scaled_point_list;
	for (auto a = smoothData.begin(); a != smoothData.end(); a++)
	{
		double xCoordinate = a->first_of_tail()->head().asNumber() * data_prop.at("xScale");
		double yCoordinate = -1 * a->tail()->head().asNumber() * data_prop.at("yScale");
		scaled_point_list.push_back(makePoint(xCoordinate, yCoordinate, 0));
	}

	for (unsigned int i = 0; i < scaled_point_list.size() - 1; i++)
	{
		result.pushback(makeLine(scaled_point_list[i], scaled_point_list[i + 1]));
	}

	for (auto a = option_list.tailConstBegin(); a != option_list.tailConstEnd(); a++)
	{
		checkAndMakeOptionList(*a, result, text_scale, data_prop);
	}

	get_borderLine(data_prop, result);
	addAlAuOlOu(data_prop, result, text_scale);
	return result;
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env) {

	if (env.InterruptSig != nullptr)
	{
		throw SemanticError("Error: interpreter kernel interrupted");
		env.InterruptSig = nullptr;
	}

	if (m_head.asSymbol() == "continuous-plot")
		return handle_continuousplot(env);

	if (m_head.asSymbol() == "map")
		return handle_map(env);

	else if (m_head.asSymbol() == "apply")
		return handle_apply(env);

	else if (m_head.asSymbol() == "set-property")
		return handle_setprop(env);

	else if (m_head.asSymbol() == "get-property")
		return handle_getprop(env);

	else if (m_tail.empty() && m_head.asSymbol() != "list") {
		return handle_lookup(m_head, env);
	}
	// handle begin special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "begin") {
		return handle_begin(env);
	}
	// handle define special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "define") {
		return handle_define(env);
	}
	// handle lambda special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "lambda") {
		return handle_lambda(env);
	}
	// else attempt to treat as procedure
	else {
		std::vector<Expression> results;
		for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
			results.push_back(it->eval(env));
		}
		return apply(m_head, results, env);
	}
}


std::ostream & operator<<(std::ostream & out, const Expression & exp) {
	if (exp.head().isNone())
	{
		out << "NONE";
	}
	else
	{
		out << "(";

		if (exp.isHeadSymbol() && (exp.head().asSymbol() == "list"))
		{
			if (exp.head().isInsideLambda())
				out << "list ";

			for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e)
				if (e == exp.tailConstBegin()) out << *e;
				else out << " " << *e;
		}

		else if (exp.isHeadSymbol() && (exp.head().asSymbol() == "lambda"))
		{
			for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e)
				if (e == exp.tailConstBegin()) out << *e;
				else out << " " << *e;
		}

		else
		{
			out << exp.head();
			for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
				out << " " << *e;
			}
		}

		out << ")";
	}
	return out;
}


bool Expression::operator==(const Expression & exp) const noexcept {

	bool result = (m_head == exp.m_head);

	result = result && (m_tail.size() == exp.m_tail.size());

	if (result) {
		for (auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
			(lefte != m_tail.end()) && (righte != exp.m_tail.end());
			++lefte, ++righte) {
			result = result && (*lefte == *righte);
		}
	}

	return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept {

	return !(left == right);
}