#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include "startup_config.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "cntlc_tracer.hpp"
typedef message_queue<std::string> MessageQueueStr;

void repl(Interpreter interp);
void prompt() {
	std::cout << "\nplotscript> ";
}

std::string readline() {
	std::string line;
	std::getline(std::cin, line);

	return line;
}

void error(const std::string & err_str) {
	std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str) {
	std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream, std::string filename) {

	Interpreter interp;

	if (!interp.parseStream(stream)) {
		error("Invalid Program. Could not parse.");
		return EXIT_FAILURE;
	}
	else {
		try {
			Expression exp = interp.evaluate();
			std::cout << exp << std::endl;
		}
		catch (const SemanticError & ex) {
			std::cerr << ex.what() << std::endl;
			return EXIT_FAILURE;
		}
	}

	if (filename == STARTUP_FILE)
		repl(interp);

	return EXIT_SUCCESS;
}

int eval_from_file(std::string filename) {

	std::ifstream ifs(filename);

	if (!ifs) {
		error("Could not open file for reading.");
		return EXIT_FAILURE;
	}

	return eval_from_stream(ifs, filename);
}

int eval_from_command(std::string argexp) {

	std::istringstream expression(argexp);

	return eval_from_stream(expression, "no_file");
}

void ProcessData(MessageQueueStr * msgIn, MessageQueueStr * msgOut, Interpreter * interp)
{
	while (1)
	{
		std::string popMessage;
		msgIn->wait_and_pop(popMessage);
		if (popMessage == "%stop") break;
		std::istringstream expMsg(popMessage);
		std::ostringstream textOutput;
		if (!interp->parseStream(expMsg))
		{
			msgOut->push("Invalid Expression. Could not parse.");
		}
		else
		{
			try
			{
				Expression exp = interp->evaluate();
				textOutput << exp;
				msgOut->push(textOutput.str());
			}
			catch (const SemanticError & ex)
			{
				textOutput << ex.what();
				msgOut->push(textOutput.str());
			}
		}
	}
}

// A REPL is a repeated read-eval-print loop
void repl(Interpreter interp) {
	bool reset = false;
	install_handler();
	while (1)
	{
		global_status_flag = 0;
		Interpreter InscopeInterp(interp);
		MessageQueueStr * msgIn = new MessageQueueStr();
		MessageQueueStr * msgOut = new MessageQueueStr();
		bool KernalRunning = true;
		std::thread * worker;
		worker = new std::thread(ProcessData, msgIn, msgOut, &InscopeInterp);
		std::string textOutput;
		while (!std::cin.eof()) {
			if (reset)
			{
				reset = false;
				break;
			}
			prompt();
			std::string line = readline();
			if (line.empty()) continue;
			if (line == "%exit")
			{
				msgIn->push("%stop");
				worker->join();
				return;
			}
			if (line == "%stop" && KernalRunning == true)
			{
				msgIn->push(line);
				worker->join();
				KernalRunning = false;
			}
			else if (line == "%start" && KernalRunning == false)
			{
				worker = new std::thread(ProcessData, msgIn, msgOut, &InscopeInterp);
				KernalRunning = true;
			}
			else if (line == "%reset")
			{
				if (KernalRunning == true)
				{
					msgIn->push("%stop");
					worker->join();
				}
				reset = true;
			}
			else if (KernalRunning == false)
			{
				std::cout << "Error: interpreter kernel not running";
				continue;
			}
			else
			{
				msgIn->push(line);
				while (!msgOut->try_pop(textOutput))
				{
					if (global_status_flag > 0) {
						global_status_flag = 0;
						std::cout << "Error: interpreter kernel interrupted";
						msgIn->push("%stop");
						worker->detach();
						worker->~thread();
						msgIn = new MessageQueueStr();
						msgOut = new MessageQueueStr();
						InscopeInterp = interp;
						worker = new std::thread(ProcessData, msgIn, msgOut, &InscopeInterp);
						break;
					}
					if (msgOut->try_pop(textOutput))
						break;
				}
				std::cout << textOutput;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc == 2) {
		return eval_from_file(argv[1]);
	}
	else if (argc == 3) {
		if (std::string(argv[1]) == "-e") {
			return eval_from_command(argv[2]);
		}
		else {
			error("Incorrect number of command line arguments.");
		}
	}
	else {
		eval_from_file(STARTUP_FILE);
	}

	return EXIT_SUCCESS;
}

