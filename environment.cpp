#include "environment.hpp"

#include <cassert>
#include <cmath>

#include "environment.hpp"
#include "semantic_error.hpp"


/************************************************************************************************************************************
Helper Functions
**************************************************************************************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs) {
	return args.size() == nargs;
}

std::unordered_map<std::string, double> checkAndScalePoints(Expression points)
{
	double xMax = -9999, xMin = 9999, yMax = -9999, yMin = 9999;

	for (auto a = points.tailConstBegin(); a != points.tailConstEnd(); a++)
	{
		if (!(a->tailSize() == 2))
		{
			throw SemanticError("Error in handle makeLollipopLine: invalid number of arguments.");
		}
		if (a->head().asSymbol() != "list")
		{
			throw SemanticError("Error in handle makeLollipopLine: arguments is not a point.");
		}
		Expression temp = *a;
		Expression * first = temp.first_of_tail();
		Expression * second = temp.tail();

		if (!first->isHeadNumber() || !second->isHeadNumber())
		{
			throw SemanticError("Error in handle makeLollipopLine: at least argument is not a point.");
		}
		double xCoordinate = first->head().asNumber();
		double yCoordinate = second->head().asNumber();

		xMax = std::max(xMax, xCoordinate);
		xMin = std::min(xMin, xCoordinate);
		yMax = std::max(yMax, yCoordinate);
		yMin = std::min(yMin, yCoordinate);
	}
	std::unordered_map<std::string, double> result;
	result.emplace("xMax", xMax);
	result.emplace("xMin", xMin);
	result.emplace("yMax", yMax);
	result.emplace("yMin", yMin);
	result.emplace("xMid", (xMax + xMin) / 2);
	result.emplace("yMid", (yMax + yMin) / 2);
	result.emplace("xScale", BOXSCALE / (xMax - xMin));
	result.emplace("yScale", BOXSCALE / (yMax - yMin));
	return result;
}

// get the scale for the text
double getTextScale(Expression options)
{
	double default_scale = 1;

	for (auto a = options.tailConstBegin(); a != options.tailConstEnd(); a++)
	{
		Expression temp = *a;
		if (temp.first_of_tail()->isHeadStringConstant())
		{
			if (temp.first_of_tail()->head().asStringConstant() == "text-scale" && temp.tail()->isHeadNumber())
				return temp.tail()->head().asNumber();
		}
	}
	return default_scale;
}

// make a point with default property given x and y location
Expression makePoint(double x, double y, double point_size)
{
	Atom point_obj = Atom("point"); point_obj.setStringType();

	Expression point(Atom("list"));
	point.add_property("object-name", point_obj);
	point.add_property("size", Atom(point_size));
	point.append(x);
	point.append(y);

	return point;
}

// make a line with default property given 2 points
Expression makeLine(Expression point1, Expression point2)
{
	Atom line_name = Atom("line"); line_name.setStringType();

	Expression line(Atom("list"));
	line.add_property("object-name", line_name);
	line.add_property("thickness", Expression(Atom(0)));
	line.pushback(point1);
	line.pushback(point2);

	return line;
}

// make text with default property given text, position, scale, and rotation
Expression makeText(std::string text_message, Expression position, double scale, double rotaion)
{
	Atom object_type("text"); object_type.setStringType();
	Atom object_message(text_message); object_message.setStringType();

	Expression text_object(object_message);
	text_object.add_property("object-name", object_type);
	text_object.add_property("position", position);
	text_object.add_property("text-scale", Expression(Atom(scale)));
	text_object.add_property("text-rotation", Expression(Atom(rotaion)));
	return text_object;
}

// get data for the border Line
void get_borderLine(std::unordered_map<std::string, double> data, Expression & result)
{
	double xMax_S = data.at("xScale") * data.at("xMax");
	double xMin_S = data.at("xScale") * data.at("xMin");

	double yMax_S = -1 * data.at("yScale") * data.at("yMax");
	double yMin_S = -1 * data.at("yScale") * data.at("yMin");

	double yZero = 0;
	double xZero = 0;
	
	Expression topleft_coordinate = makePoint(xMin_S, yMax_S, 0);
	Expression topright_coordinate = makePoint(xMax_S, yMax_S, 0);
	Expression bottomleft_coordinate = makePoint(xMin_S, yMin_S, 0);
	Expression bottomright_coordinate = makePoint (xMax_S, yMin_S, 0);

	Expression midtop_coordinate = makePoint(xZero, yMax_S, 0);
	Expression midbot_coordinate = makePoint(xZero, yMin_S, 0);
	Expression midleft_coordinate = makePoint(xMin_S, yZero, 0);
	Expression midright_coordinate = makePoint(xMax_S, yZero, 0);


	Expression top_border = makeLine(topleft_coordinate, topright_coordinate);
	Expression bottom_border = makeLine (bottomleft_coordinate, bottomright_coordinate);
	Expression left_border = makeLine (topleft_coordinate, bottomleft_coordinate);
	Expression right_border = makeLine (topright_coordinate, bottomright_coordinate);

	Expression y_axis = makeLine (midbot_coordinate, midtop_coordinate);
	Expression x_axis = makeLine (midleft_coordinate, midright_coordinate);

	result.pushback(top_border);
	result.pushback(bottom_border);
	result.pushback(left_border);
	result.pushback(right_border);


	if (yZero < yMin_S && yZero > yMax_S)
	{
		result.pushback(x_axis);
	}
	if (xZero > xMin_S && xZero < xMax_S)
	{
		result.pushback(y_axis);

	}
}

// create a lolipop given a point.
void makeLollipopLine(Expression point, Expression & result, std::unordered_map<std::string, double> data)
{
	double yMax_S = -1 * data.at("yScale") * data.at("yMax");
	double yMin_S = -1 * data.at("yScale") * data.at("yMin");

	double xCoordinate = point.first_of_tail()->head().asNumber() * data.at("xScale");
	double yCoordinate = -1 * point.tail()->head().asNumber() * data.at("yScale");
	double yZero = -1 * 0 * data.at("yScale");

	double bot_point = 0;
	if (yZero < yMin_S && yZero > yMax_S)
	{
		bot_point = yZero;
	}
	else if (yMin_S < yZero)
	{
		bot_point = yMin_S;
	}
	else
	{
		bot_point = yMax_S;
	}

	Expression first_point = makePoint(xCoordinate, yCoordinate, POINTSIZE);
	Expression second_point = makePoint(xCoordinate, bot_point, 0);

	Expression line = makeLine(makePoint(xCoordinate, yCoordinate, 0), second_point);

	result.pushback(first_point);
	result.pushback(line);
}

// checking to make sure all the list are correct.
void checkAndMakeOptionList(Expression option, Expression & result, double text_scale, std::unordered_map<std::string, double> data)
{

	if (!(option.tailSize() == 2))
	{
		throw SemanticError("Error in checkAndMakeOptionList: invalid number of arguments.");
	}
	if (!option.head().isSymbol())
	{
		throw SemanticError("Error in checkAndMakeOptionList: option is not a symol kind.");
	}
	if (option.head().asSymbol() != "list")
	{
		throw SemanticError("Error in checkAndMakeOptionList : option is not a list.");
	}
	if (!option.first_of_tail()->isHeadStringConstant())
	{
		throw SemanticError("Error in checkAndMakeOptionList : first argument is not string");
	}


	double xMax_S = data.at("xScale") * data.at("xMax");
	double xMin_S = data.at("xScale") * data.at("xMin");

	double yMax_S = -1 * data.at("yScale") * data.at("yMax");
	double yMin_S = -1 * data.at("yScale") * data.at("yMin");
	double yMid_S = (yMax_S + yMin_S) / 2;
	double xMid_S = (xMax_S + xMin_S) / 2;

	Expression Object;
	if (option.first_of_tail()->head().asStringConstant() == "title" && option.tail()->isHeadStringConstant())
	{
		Object = makeText(option.tail()->head().asStringConstant(), makePoint(xMid_S, yMax_S - 3, 0), text_scale, 0);
	}
	else if (option.first_of_tail()->head().asStringConstant() == "abscissa-label" && option.tail()->isHeadStringConstant())
	{
		Object = makeText(option.tail()->head().asStringConstant(), makePoint(xMid_S, yMin_S + 3, 0), text_scale, 0);
	}
	else if (option.first_of_tail()->head().asStringConstant() == "ordinate-label" && option.tail()->isHeadStringConstant())
	{
		Object = makeText(option.tail()->head().asStringConstant(), makePoint( xMin_S - 3, yMid_S, 0), text_scale, -std::atan2(0, -1) / 2);
	}
	else if (option.first_of_tail()->head().asStringConstant() == "text-scale" && option.tail()->isHeadNumber())
	{
		return;
	}
	else
	{
		throw SemanticError("Error in checkAndMakeOptionList: invalid argument.");
	}
	result.pushback(Object);
}

void addAlAuOlOu(std::unordered_map<std::string, double> data, Expression & result, double text_scale)
{

	double xMin = data.at("xMin");
	double xMax = data.at("xMax");
	double yMin = data.at("yMin");
	double yMax = data.at("yMax");

	double xMax_S = data.at("xScale") * data.at("xMax");
	double xMin_S = data.at("xScale") * data.at("xMin");

	double yMax_S = -1 * data.at("yScale") * data.at("yMax");
	double yMin_S = -1 * data.at("yScale") * data.at("yMin");

	std::ostringstream out1;
	out1.precision(2);
	out1 << xMin;
	result.pushback(makeText(out1.str(), makePoint(xMin_S, yMin_S + 2, 0), text_scale, 0));

	std::ostringstream out2;
	out2.precision(2);
	out2 << xMax;
	result.pushback(makeText(out2.str(), makePoint(xMax_S, yMin_S + 2, 0), text_scale, 0));

	std::ostringstream out3;
	out3.precision(2);
	out3 << yMin;
	result.pushback(makeText(out3.str(), makePoint(xMin_S - 2, yMin_S, 0), text_scale, 0));

	std::ostringstream out4;
	out4.precision(2);
	out4 << yMax;
	result.pushback(makeText(out4.str(), makePoint(xMin_S - 2, yMax_S, 0), text_scale, 0));

}
/***************************************************************************************************************************************
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**************************************************************************************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args) {
	args.size(); // make compiler happy we used this parameter
	return Expression();
};

ComplexNumber addcomplex(const std::vector <Expression> &args)
{
	ComplexNumber result(0, 0);
	for (auto & a : args)
	{// check all aruments are numbers or complex, while adding
		if (a.isHeadNumber())
		{
			result.real(std::real(result) + a.head().asNumber());
		}
		else if (a.isHeadComplex())
		{
			// add both real and imag
			result.real(std::real(result) + a.head().asRealNumber());
			result.imag(std::imag(result) + a.head().asImaginaryNumber());
		}
		else
		{
			throw SemanticError("Error in call to add complex numbers: argument is not a number or complex number");
		}
	}
	return result;
}

Expression add(const std::vector<Expression> & args) {

	
	double result = 0;

	bool containcomplex = false;
	for (auto & a : args) {
		if (a.isHeadComplex())
			containcomplex = true;
	}
	if (containcomplex)
	{
		return Expression(addcomplex(args));
	}
	else
	{
		for (auto & a : args) {
			// check all aruments are numbers, while adding
			if (a.isHeadNumber()) {
				result += a.head().asNumber();
			}
			else {
				throw SemanticError("Error in call to add, argument not a number");
			}
		}
	}
	return Expression(result);
};


ComplexNumber mulcomplex(const std::vector <Expression> &args)
{
	ComplexNumber result;
	bool setfirstcomplex = false;
	double realnumber = 1;
	for (auto & a : args)
	{// check all aruments are numbers or complex, while multiplying
		if (a.isHeadNumber())
		{
			realnumber = realnumber * a.head().asNumber();
		}
		else if (a.isHeadComplex())
		{
			if (!setfirstcomplex)
			{
				result = a.head().asComplexNumber();
				setfirstcomplex = true;
			}
			else
				result = result * a.head().asComplexNumber();
		}
		else
			throw SemanticError("Error in call to multiply with complex numbers: argument not a number or complex number");
	}
	result.imag(std::imag(result) * realnumber);
	result.real(std::real(result) * realnumber);
	return result;
}

Expression mul(const std::vector<Expression> & args) {

	// check all aruments are numbers, while multiplying
	double result = 1;

	bool containcomplex = false;
	for (auto & a : args) {
		if (a.isHeadComplex())
			containcomplex = true;
	}
	if (containcomplex)
	{
		return Expression(mulcomplex(args));
	}
	else
	{
		for (auto & a : args) {
			if (a.isHeadNumber()) {
				result *= a.head().asNumber();
			}
			else {
				throw SemanticError("Error in call to mul, argument not a number");
			}
		}
	}
	return Expression(result);
};

ComplexNumber subnegcomplex(const std::vector<Expression> & args)
{
	ComplexNumber result(0, 0);
	if (nargs_equal(args, 1))
	{
			result.imag(-(args[0].head().asImaginaryNumber()));
			result.real(-(args[0].head().asRealNumber()));
	}
	else if (nargs_equal(args, 2))
	{
		if (args[0].isHeadComplex() && args[1].isHeadComplex())
		{
			result.imag(args[0].head().asImaginaryNumber() - args[1].head().asImaginaryNumber());
			result.real(args[0].head().asRealNumber() - args[1].head().asRealNumber());
		}
		else if (args[0].isHeadComplex() && args[1].isHeadNumber())
		{
			result.real(args[0].head().asRealNumber() - args[1].head().asNumber());
			result.imag(args[0].head().asImaginaryNumber());
		}
		else if (args[0].isHeadNumber() && args[1].isHeadComplex())
		{
			result.real(args[0].head().asNumber() - args[1].head().asRealNumber());
			result.imag(0-args[1].head().asImaginaryNumber());
		}
		else 
			throw SemanticError("Error in call to subneg with complex numbers: invalid argument.");
	}
	else
		throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");

	return result;

}

Expression subneg(const std::vector<Expression> & args) {

	double result = 0;

	bool containcomplex = false;
	for (auto & a : args) {
		if (a.isHeadComplex())
			containcomplex = true;
	}
	if (containcomplex)
		return Expression(subnegcomplex(args));

	else
	{
		// preconditions
		if (nargs_equal(args, 1)) {
			if (args[0].isHeadNumber()) {
				result = -args[0].head().asNumber();
			}
			else {
				throw SemanticError("Error in call to negate: invalid argument.");
			}
		}
		else if (nargs_equal(args, 2)) {
			if ((args[0].isHeadNumber()) && (args[1].isHeadNumber())) {
				result = args[0].head().asNumber() - args[1].head().asNumber();
			}
			else {
				throw SemanticError("Error in call to subtraction: invalid argument.");
			}
		}
		else {
			throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
		}
	}
	return Expression(result);
};


ComplexNumber divcomplex(const std::vector<Expression> & args)
{
	ComplexNumber result(0, 0);

	if (nargs_equal(args, 2))
	{
		if (args[0].isHeadComplex() && args[1].isHeadComplex())
		{
			result = args[0].head().asComplexNumber() / args[1].head().asComplexNumber();
		}
		else if (args[0].isHeadComplex() && args[1].isHeadNumber())
		{
			result = args[0].head().asComplexNumber() / args[1].head().asNumber();
		}
		else if (args[0].isHeadNumber() && args[1].isHeadComplex())
		{
			result = args[0].head().asNumber() / args[1].head().asComplexNumber();
		}
		else
			throw SemanticError("Error in call to division with complex numbers: invalid argument.");
	}
	else if (nargs_equal(args, 1))
	{
		result = 1.0 / args[0].head().asComplexNumber();
	}
	else
		throw SemanticError("Error in call to division with complexx numbers: invalid number of arguments.");
	return result;
}

Expression div(const std::vector<Expression> & args) {

	double result = 0;
	bool containcomplex = false;

	for (auto & a : args) {
		if (a.isHeadComplex())
			containcomplex = true;
	}
	if (containcomplex)
		return Expression(divcomplex(args));
	else
	{
		if (nargs_equal(args, 2)) {

			if (!(args[0].isHeadNumber() && args[1].isHeadNumber())) {
				throw SemanticError("Error in call to division: invalid argument.");
			}
				result = args[0].head().asNumber() / args[1].head().asNumber();
		}

		else if (nargs_equal(args, 1))
		{
			if (!(args[0].isHeadNumber()))
				throw SemanticError("Error in call to division: invalid argument.");

			result = 1.0 / args[0].head().asNumber();
		}

		else {
			throw SemanticError("Error in call to division: invalid number of arguments.");
		}
	}
	return Expression(result);
};

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const ComplexNumber IMAGINARYNUM(0, 1);

ComplexNumber negsquareroot(const std::vector<Expression> &args)
{
	ComplexNumber result(0,0);

	if (args[0].isHeadNumber())
		result = std::sqrt(ComplexNumber(args[0].head().asNumber()));
	else if (args[0].isHeadComplex())
		result = std::sqrt(args[0].head().asComplexNumber());
	
	return result;
}

Expression squareroot(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 1))
	throw SemanticError("Error in call to Squareroot : invalid number of argument.");

	double result = 0;
	if (args[0].isHeadNumber() || args[0].isHeadComplex())
	{
		if (args[0].head().asNumber() >= 0 && args[0].isHeadNumber())
			result = std::sqrt(args[0].head().asNumber());
		else 
			return Expression(negsquareroot(args));
	}
	else
		throw SemanticError("Error in call to squareroot: arguments is not number or complex number.");

	return Expression(result);
}


ComplexNumber tothepowercomplex(const std::vector<Expression> &args)
{
	ComplexNumber result(0, 0);
	if (args[0].isHeadComplex() && args[1].isHeadNumber())
		result = std::pow(args[0].head().asComplexNumber(), args[1].head().asNumber());
	else if (args[0].isHeadNumber() && args[1].isHeadComplex())
		result = std::pow(args[0].head().asNumber(), args[1].head().asComplexNumber());
	else if (args[0].isHeadComplex() && args[1].isHeadComplex())
		result = std::pow(args[0].head().asComplexNumber(), args[1].head().asComplexNumber());
	else 
		throw SemanticError("Error in call to power : arguments is not number or complex number.");

	return result;
}

Expression tothepower(const std::vector<Expression> &args)
{
	double result = 0;

		if (nargs_equal(args, 2))
		{
			if (args[0].isHeadNumber() && args[1].isHeadNumber())
				result = std::pow(args[0].head().asNumber(), args[1].head().asNumber());
			else
				return Expression(tothepowercomplex(args));
		}
		else
			throw SemanticError("Error in call to Power : More than two arguments");

	return Expression(result);
}

Expression naturelog(const std::vector<Expression> &args)
{

	if (!nargs_equal(args, 1))
		throw SemanticError("Error in call to NatureLog : More than one argument");

	if (!args[0].head().isNumber())
		throw SemanticError("Error in call to NatureLog : Argument is not a number");

	if (args[0].head().asNumber() < 0)
		throw SemanticError("Error in call to NatureLog : Argument is a negative number");
	
	return Expression(std::log(args[0].head().asNumber()));
}

Expression sine(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 1))
		throw SemanticError("Error in call to Sine : More than one argument ");	

	if (!args[0].head().isNumber())
		throw SemanticError("Error in call to Sine : Argument is not a number");

	return Expression(std::sin(args[0].head().asNumber()));
}

Expression cosine(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 1))
		throw SemanticError("Error in call to Cosine : More than one argument ");

	if (!args[0].head().isNumber())
		throw SemanticError("Error in call to Cosine : Argument is not a number");

	return Expression(std::cos(args[0].head().asNumber()));
}

Expression tangent(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 1))
		throw SemanticError("Error in call to Tangent : More than one argument ");

	if (!args[0].head().isNumber())
		throw SemanticError("Error in call to Tangent : Argument is not a number");

	return Expression(std::tan(args[0].head().asNumber()));
}

Expression realnumber(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 1))
		throw SemanticError("Error in call to get real number : number of arguments is in correct ");

	if (!args[0].isHeadComplex())
		throw SemanticError("Error in call to get real number : argument is not a complex number");

	return Expression(args[0].head().asRealNumber());
}

Expression imagnumber(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 1))
		throw SemanticError("Error in call to get imaginary number : number of arguments is in correct ");

	if (!args[0].isHeadComplex())
		throw SemanticError("Error in call to get imaginary number : argument is not a complex number");

	return Expression(args[0].head().asImaginaryNumber());
}

Expression magnitude(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 1))
		throw SemanticError("Error in call to get magnitude : number of arguments is in correct");

	if (!args[0].isHeadComplex())
		throw SemanticError("Error in call to get magnitude : argument is not a complex number");

	double result = 0;
	result = std::sqrt(std::pow(args[0].head().asRealNumber(),2) + std::pow(args[0].head().asImaginaryNumber(),2));

	return Expression(result);
}


Expression argument(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 1))
		throw SemanticError("Error in call to get angle argument : More than one argument");

	if (!args[0].isHeadComplex())
		throw SemanticError("Error in call to get angle argument : argument is not a complex number");		

	return Expression(std::arg(args[0].head().asComplexNumber()));
}

Expression conjugate(const std::vector<Expression> &args)
{
	ComplexNumber result(0, 0);
	if (!nargs_equal(args, 1))
		throw SemanticError("Error in call to get conjugate : More than one argument inside magnitude");

	if (!args[0].isHeadComplex())
		throw SemanticError("Error in call to get conjugate : argument is not a complex number");

	return Expression(std::conj(args[0].head().asComplexNumber()));
}

Environment::Environment() {

	reset();
}

bool Environment::is_known(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const {

	Expression exp;

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && (result->second.type == ExpressionType)) {
			exp = result->second.exp;
		}
	}

	return exp; 
}

Expression Environment::get_UserDefineProc(const Atom & sym) const {

	Expression exp;

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && ((result->second.exp.head().asSymbol() == "lambda"))) {
			exp = result->second.exp;
		}
	}
	return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp) {

	if (!sym.isSymbol()) {
		throw SemanticError("Attempt to add non-symbol to environment");
	}

	// error if overwriting symbol map
	if (this->is_proc(sym.asSymbol())) {
		throw SemanticError("Attempt to overwrite symbol in environemnt");
	}

	auto result = envmap.find(sym.asSymbol());
	// check to see expression is already there
	if ((result != envmap.end()) && (result->second.type == ExpressionType))
		result->second.exp = exp;
	else 
		envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));

}

Expression Environment::make_list(const Expression & exp)
{	
	Expression result(Atom("list"));
	result.append(exp.head());

	if (!exp.isTailEmpty())
	{
		for (auto a = exp.tailConstBegin(); a != exp.tailConstEnd(); a++)
		{
			std::string s = a->head().asSymbol();
			if (s == "define" || s == "begin")
				throw SemanticError("Error during evaluation (handle lambda): attempt to redefine a special-form");
			if (is_proc(a->head())) {
				throw SemanticError("Error during evaluation (handle lambda): attempt to redefine a built-in procedure");
			}

			result.pushback(*a);
		}
	}

	return result;
}

bool Environment::is_proc(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ProcedureType);
}

bool Environment::is_userDefine(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.exp.head().asSymbol() == "lambda");
}

Procedure Environment::get_proc(const Atom & sym) const {

	//Procedure proc = default_proc;

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && (result->second.type == ProcedureType)) {
			return result->second.proc;
		}
	}

	return default_proc;
}

Expression makelist(const std::vector<Expression> &args)
{
	Expression result(Atom("list"));
		for (auto a : args)
			result.pushback(a);;
	return result;
}

Expression firstinlist(const std::vector<Expression> &args)
{
	Expression result;
	if (!nargs_equal(args, 1))
		throw SemanticError("Error: incorrect number of argument is call to first");

	if (args[0].head().asSymbol() != "list")
		throw SemanticError("Error: argument to first is not a list");

	if (args[0].isTailEmpty())
		throw SemanticError("Error: argument to first is an empty list");

	return  *args[0].tailConstBegin();
}

Expression restoflist(const std::vector<Expression> &args)
{
	Expression result(Atom("list"));
	if (!nargs_equal(args, 1))
		throw SemanticError("Error: incorrect number of argument is call to rest");

	if (args[0].head().asSymbol() != "list")
		throw SemanticError("Error: argument to rest is not a list");

	if (args[0].isTailEmpty())
		throw SemanticError("Error: argument to rest is an empty list");

	if (args[0].tailSize() > 2)
		for (auto a = args[0].tailConstBegin(); a != args[0].tailConstEnd(); a++)
			if (a != args[0].tailConstBegin())
				result.pushback(*a);

	return result;
}

Expression listsize(const std::vector<Expression> &args)
{
	unsigned int result = 0;
	if (!nargs_equal(args, 1))
		throw SemanticError("Error: more than one argument is call to length");

	if (args[0].head().asSymbol() != "list")
		throw SemanticError("Error: argument to length is not a list");

	if (!args[0].isTailEmpty())
		for (auto a = args[0].tailConstBegin(); a != args[0].tailConstEnd();a++)
			result += 1;

	return Expression(result);
}

Expression appending(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 2))
		throw SemanticError("Error in call to append: invalid number of arguments.");

	if (!(args[0].head().asSymbol() == "list"))
		throw SemanticError("Error: first argument to append not a list");

	Expression result(Atom("list"));

	for (auto a = args[0].tailConstBegin(); a != args[0].tailConstEnd(); a++)
		result.pushback(*a);

	result.pushback(args[1]);
		
	return result;
}

Expression joinlist(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 2))
		throw SemanticError("Error in call to append: invalid number of arguments.");

	if (!(args[0].head().asSymbol() == "list" && args[1].head().asSymbol() == "list"))
		throw SemanticError("Error: argument to join not a list");

	Expression result(Atom("list"));

	for (int i = 0; i < 2; i++)
		for (auto a = args[i].tailConstBegin(); a != args[i].tailConstEnd(); a++)
			result.pushback(*a);

	return result;
}

Expression rangelist(const std::vector<Expression> &args)
{
	if (!nargs_equal(args, 3))
		throw SemanticError("Error in call to range: invalid number of arguments.");

	for (auto a : args)
		if (!(a.isHeadNumber()))
			throw SemanticError("Error in call to range: invalid argument");

	if (args[0].head().asNumber() > args[1].head().asNumber())
		throw SemanticError("Error: begin greater than end in range");

	if (args[2].head().asNumber() <= 0 )
		throw SemanticError("Error: negative or zero increment in range");

	Expression result(Atom("list"));
	for (double i = args[0].head().asNumber(); i <= args[1].head().asNumber(); i += args[2].head().asNumber())
		result.pushback(Expression(i));

	return result;
}

Expression discreteplot(const std::vector<Expression> &args)
{
	if (args.size() != 2)
	{
		throw SemanticError("Error in handle discrete plot: invalid number of arguments.");
	}
	Expression data_list = args[0];
	Expression option_list = args[1];

	if (data_list.head().asSymbol() != "list")
	{
		throw SemanticError("Error in handle discrete plot: first argument is not a list.");
	}
	if (option_list.head().asSymbol() != "list")
	{
		throw SemanticError("Error in handle discrete plot: second argument is not a list.");
	}
	if (option_list.tailSize() > 4 || option_list.tailSize() < 0)
	{
		throw SemanticError("Error in handle discrete plot: second argument has more than 4 tails.");
	}
	Expression result(Atom("list"));

	std::unordered_map<std::string, double> data_prop = checkAndScalePoints(data_list);

	for (auto a = data_list.tailConstBegin(); a != data_list.tailConstEnd(); a++)
	{
		makeLollipopLine(*a, result, data_prop);
	}
	get_borderLine(data_prop, result);

	double text_scale = getTextScale(option_list);

	for (auto a = option_list.tailConstBegin(); a != option_list.tailConstEnd(); a++)
	{
		checkAndMakeOptionList(*a, result, text_scale, data_prop);
	}
	addAlAuOlOu(data_prop, result, text_scale);

	return result;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset() {

	envmap.clear();

	// Built-In value of pi
	envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

	// Built-In value of e
	envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

	// Build-In value of I
	envmap.emplace("I", EnvResult(ExpressionType, Expression(IMAGINARYNUM)));

	// Making A list 
	envmap.emplace("list", EnvResult(ProcedureType, makelist));

	// getting first element of the list
	envmap.emplace("first", EnvResult(ProcedureType, firstinlist));

	// getting from second element to the end of the list
	envmap.emplace("rest", EnvResult(ProcedureType, restoflist));

	// getting number of items inside a list
	envmap.emplace("length", EnvResult(ProcedureType, listsize));

	// appends the expression of second arg to first list arg
	envmap.emplace("append", EnvResult(ProcedureType, appending));

	// join 2 list together
	envmap.emplace("join", EnvResult(ProcedureType, joinlist));

	// join 2 list together
	envmap.emplace("range", EnvResult(ProcedureType, rangelist));

	// Procedure: add;
	envmap.emplace("+", EnvResult(ProcedureType, add));
	
	// Procedure: subneg;
	envmap.emplace("-", EnvResult(ProcedureType, subneg));

	// Procedure: mul;
	envmap.emplace("*", EnvResult(ProcedureType, mul));

	// Procedure: div;
	envmap.emplace("/", EnvResult(ProcedureType, div));

	//Procedure: sqrt
	envmap.emplace("sqrt", EnvResult(ProcedureType, squareroot));

	//Procedure: ^
	envmap.emplace("^", EnvResult(ProcedureType, tothepower));

	//Procedure: ln
	envmap.emplace("ln", EnvResult(ProcedureType, naturelog));

	//Procedure: sine
	envmap.emplace("sin", EnvResult(ProcedureType, sine));

	//Procedure: cosine
	envmap.emplace("cos", EnvResult(ProcedureType, cosine));

	//Procedure: tangent
	envmap.emplace("tan", EnvResult(ProcedureType, tangent));

	//Procedure: real number
	envmap.emplace("real", EnvResult(ProcedureType, realnumber));

	//Procedure: imaginary number
	envmap.emplace("imag", EnvResult(ProcedureType, imagnumber));

	//Procedure: magnitude
	envmap.emplace("mag", EnvResult(ProcedureType, magnitude));

	//Procedure: argument phase or angle
	envmap.emplace("arg", EnvResult(ProcedureType, argument));

	//Procedure: conjugate
	envmap.emplace("conj", EnvResult(ProcedureType, conjugate));

	//Procedure: argument phase or angle
	envmap.emplace("arg", EnvResult(ProcedureType, argument));

	//Procedure: discrete plot
	envmap.emplace("discrete-plot", EnvResult(ProcedureType, discreteplot));
}

void Environment::setInterruptSignal(MessageQueueStr * signal)
{
	InterruptSig = signal;
}
