#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP
#include"input_widget.hpp"
#include"output_widget.hpp"
#include"interpreter.hpp"
#include <QWidget>
#include <QPushButton>
#include <thread>
#include "startup_config.hpp"


typedef struct
{
	Expression Exp;
	std::string ErrMsg;
	bool valid = false;
} Data;

typedef message_queue<std::string> MessageQueueStr;
typedef message_queue<Data> MessageQueueData;

class NotebookApp : public QWidget
{
	Q_OBJECT
private:
	InputWidget * input;
	OutputWidget * output;
	QPushButton * start;
	QPushButton * stop;
	QPushButton * reset;
	QPushButton * interrupt;
	Interpreter interp;
	std::thread worker;
	int timer;
	bool thRunning = false;

	MessageQueueStr msgIn;
	MessageQueueData msgOut;
	MessageQueueStr InterruptionMessage;

	void makeConnection();
	static void ProcessData(MessageQueueStr & msgIn, MessageQueueData & msgOut, Interpreter & interp);
public:
	NotebookApp(QWidget * parent = nullptr);
	void startUp();
	Expression AnalyzeOutput(const Expression & exp);
	void processPoint(Expression exp);
	void processLine(Expression exp);
	void processText(Expression exp);
	~NotebookApp();
	void testNoteBook();
	void timerEvent(QTimerEvent *event);

signals:
	void ErrorMessage(std::string error);
	void validOutput(std::string message);
	void drawPoint(double x, double y, double pointsize);
	void drawLine(double x1, double y1, double x2, double y2, double thickness_size);
	void drawString(double x, double y, double angle, double scale, std::string text_message);
	void ClearScene();
	void clicked(bool checked = false);
private slots:
	void process(std::string value);
	void handleStartButton();
	void handleStopButton();
	void handleResetButton();
	void handleInterruptButton();


};


#endif // !NOTEBOOK_APP_HPP
