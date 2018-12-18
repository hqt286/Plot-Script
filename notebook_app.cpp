#include"notebook_app.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

#include <QWidget>
#include <QLayout>
#include <QDebug>

void NotebookApp::ProcessData(MessageQueueStr & msgIn, MessageQueueData & msgOut, Interpreter & interp)
{
	while (1)
	{
		std::string popMessage;
		msgIn.wait_and_pop(popMessage);
		if (popMessage == "%stop") break;
		std::istringstream expMsg(popMessage);
		Data OutputData;
		if (!interp.parseStream(expMsg))
		{
			OutputData.ErrMsg = "Error: Invalid Expression. Could not parse.";
			OutputData.valid = false;
			msgOut.push(OutputData);
		}
		else
		{
			try
			{
				Expression exp = interp.evaluate();
				OutputData.Exp = exp;
				OutputData.valid = true;
				msgOut.push(OutputData);
			}
			catch (const SemanticError & ex)
			{
				OutputData.ErrMsg = ex.what();
				OutputData.valid = false;
				msgOut.push(OutputData);
			}
		}
	}
}



void NotebookApp::makeConnection()
{
	QObject::connect(input, SIGNAL(Changed(std::string)), this, SLOT(process(std::string)));
	QObject::connect(this, SIGNAL(ClearScene()), output, SLOT(RecieveClearScene()));
	QObject::connect(this, SIGNAL(ErrorMessage(std::string)), output, SLOT(RecieveError(std::string)));
	QObject::connect(this, SIGNAL(validOutput(std::string)), output, SLOT(RecieveValidOutput(std::string)));
	QObject::connect(this, SIGNAL(drawPoint(double, double, double)), output, SLOT(RecieveDrawPoint(double, double, double)));
	QObject::connect(this, SIGNAL(drawLine(double, double, double, double, double)), output, SLOT(RecieveDrawLine(double, double, double, double , double)));
	QObject::connect(this, SIGNAL(drawString(double, double, double, double, std::string)), output, SLOT(RecieveDrawString(double, double, double, double, std::string)));
	QObject::connect(start, SIGNAL(clicked()), this, SLOT(handleStartButton()));
	QObject::connect(stop, SIGNAL(clicked()), this, SLOT(handleStopButton()));
	QObject::connect(reset, SIGNAL(clicked()), this, SLOT(handleResetButton()));
	QObject::connect(interrupt, SIGNAL(clicked()), this, SLOT(handleInterruptButton()));
}

void NotebookApp::handleStartButton()
{
	if (thRunning == false)
	{
		thRunning = true;
		worker = std::thread(ProcessData, std::ref(msgIn), std::ref(msgOut), std::ref(interp));
	}
}

void NotebookApp::handleStopButton()
{
	if (thRunning == true)
	{
		msgIn.push("%stop");
		worker.join();
		thRunning = false;
	}
	else
	{
		emit ErrorMessage("Error: interpreter kernel not running");
	}
}

void NotebookApp::handleResetButton()
{
	interp = Interpreter();
	startUp();
	if (thRunning == false)
	{
		thRunning = true;
		worker = std::thread(ProcessData, std::ref(msgIn), std::ref(msgOut), std::ref(interp));
	}
}

void NotebookApp::handleInterruptButton()
{
	msgIn.push("%stop");
	interp.setInterrupSig(&InterruptionMessage);
	emit ErrorMessage("Error: interpreter kernel interrupted");
}

NotebookApp::NotebookApp(QWidget * parent) : QWidget(parent)
{
	input = new InputWidget(this);
	output = new OutputWidget(this);
	start = new QPushButton("Start Kernel", this);
	start->setObjectName("start");
	stop = new QPushButton("Stop Kernel", this);
	stop->setObjectName("stop");
	reset = new QPushButton("Reset Kernel", this);
	reset->setObjectName("reset");
	interrupt = new QPushButton("Interrupt", this);
	interrupt->setObjectName("interrupt");

	auto HQlayout = new QHBoxLayout();
	HQlayout->addWidget(start);
	HQlayout->addWidget(stop);
	HQlayout->addWidget(reset);
	HQlayout->addWidget(interrupt);

	auto layout = new QVBoxLayout();
	layout->addLayout(HQlayout);
	layout->addWidget(input);
	layout->addWidget(output);

	output->setParent(this);
	setLayout(layout);
	startUp();
	makeConnection();

	thRunning = true;
	timer = QObject::startTimer(5);
	worker = std::thread(ProcessData, std::ref(msgIn), std::ref(msgOut), std::ref(interp));
}

void NotebookApp::startUp()
{
	std::ifstream in(STARTUP_FILE);
	std::string inputLine;

	if (!in)
	{
		emit ErrorMessage("Error: Can't open file " + STARTUP_FILE + ".");
	}
	else if (!interp.parseStream(in)) {
		emit ErrorMessage("Error: Invalid Expression.Could not parse.");
	}
	else {
		try {
			Expression exp = interp.evaluate();
		}
		catch (const SemanticError & ex) {
			emit ErrorMessage(ex.what());
		}
	}
}

void NotebookApp::processPoint(Expression exp)
{
	Expression size = exp.getProperty("size");
	double pointsize = size.head().asNumber();
	double x = exp.tailConstBegin()->head().asNumber();
	double y = exp.tail()->head().asNumber();

	if (pointsize < 0)
	{
		emit ErrorMessage("diameter of at least one point is negative");
		return;
	}

	emit drawPoint(x, y, pointsize);
}

void NotebookApp::processLine(Expression exp)
{
	Expression thickness = exp.getProperty("thickness");
	double thickness_size = thickness.head().asNumber();

	if (thickness_size < 0)
	{
		emit ErrorMessage("thickness of at least a line is negative");
		return ;
	}

	double coordinate[4]; int i = 0;
	auto point1 = exp.tailConstBegin();
	auto point2 = exp.tail();
	for (auto a = point1->tailConstBegin(); a != point1->tailConstEnd();a++)
	{
		coordinate[i] = a->head().asNumber();
		i++;
	}
	for (auto a = point2->tailConstBegin(); a != point2->tailConstEnd();a++)
	{
		coordinate[i] = a->head().asNumber();
		i++;
	}

	emit drawLine(coordinate[0], coordinate[1], coordinate[2], coordinate[3], thickness_size);
}

void NotebookApp::processText(Expression exp)
{
	Expression position = exp.getProperty("position");
	Expression point = position.getProperty("object-name");
	Expression rotation = exp.getProperty("text-rotation");
	Expression text_scale = exp.getProperty("text-scale");

	if (point.head().asStringConstant() != "point")
	{
		emit ErrorMessage("position of text is not a point");
		return;
	}

	double x = position.tailConstBegin()->head().asNumber();
	double y = position.tail()->head().asNumber();
	double angle = rotation.head().asNumber();
	double scale = text_scale.head().asNumber();

	if (scale < 1) scale = 1;

	std::string text_message = exp.head().asStringConstant();
	emit drawString(x, y, angle, scale, text_message);

}

NotebookApp::~NotebookApp()
{
	if (thRunning == true)
	{
		msgIn.push("%stop");
		worker.join();
	}
	delete input;
	delete output;
	input = nullptr;
	output = nullptr;
}

void NotebookApp::testNoteBook()
{
	std::string check = input->toPlainText().toStdString();
	process(check);
}

Expression NotebookApp::AnalyzeOutput(const Expression & exp)
{
	if (exp.isHeadSymbol())
		if (exp.head().asSymbol() == "lambda")
			return exp;

	Expression expPropertyType = exp.getProperty("object-name");

	if (exp.head().isNone())
	{
		emit ErrorMessage("NONE");
		return exp;
	}

	else if (expPropertyType.head().asStringConstant() == "point")
	{
		(processPoint(exp));
		return exp;
	}

	else if (expPropertyType.head().asStringConstant() == "line")
	{
		(processLine(exp));
		return exp;
	}

	else if (expPropertyType.head().asStringConstant() == "text")
	{
		(processText(exp));
		return exp;
	}

	else if (exp.isHeadSymbol() && (exp.head().asSymbol() == "list") && (exp.propertySize() == 0))
	 {
		 if (!exp.isTailEmpty())
			 for (auto a = exp.tailConstBegin(); a != exp.tailConstEnd(); a++)
				 AnalyzeOutput(*a);
	 }

	else
	{
		std::ostringstream text;
		text << exp;
		emit validOutput(text.str());
	}

	return exp;
}

//void NotebookApp::process(std::string line)
//{
//	emit ClearScene();
//	if (!line.empty())
//	{
//		std::istringstream expression(line);
//
//		if (!interp.parseStream(expression))
//			emit ErrorMessage("Error: Invalid Expression.Could not parse.");
//		else {
//			try {
//				Expression exp = interp.evaluate();
//					AnalyzeOutput(exp);
//			}
//			catch (const SemanticError & ex) {
//				emit ErrorMessage(ex.what());
//			}
//		}
//	}
//}

void NotebookApp::process(std::string line)
{
	emit ClearScene();
	if (!line.empty() && thRunning == true)
	{
		Data result;
		msgIn.push(line);
	}
	if (!line.empty() && !worker.joinable()) // thread is not working
	{
		emit ErrorMessage("Error: interpreter kernel not running");
	}
}

void NotebookApp::timerEvent(QTimerEvent *event) {
	Data result;
	if (msgOut.try_pop(result))
	{
		if (result.valid)
		{
			AnalyzeOutput(result.Exp);
		}
		else
		{
			emit ErrorMessage(result.ErrMsg);
		}
	}
	else
		QObject::timerEvent(event);

}