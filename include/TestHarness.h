#pragma once
#include "Logger.h"
#include <functional>
#include<chrono>
#include<ctime>

class TestHarness {
public:
	TestHarness();
	TestHarness(std::string);
	TestHarness(std::string, Logger::LOG_LEVELS);

	template <typename T> 
	bool Test(T);
	template <typename T, typename U> 
	bool Test(T, U);		

	void setLoggerLevel(Logger::LOG_LEVELS);
	Logger::LOG_LEVELS getLoggerLevel();

private:
	bool openLogFile();		
	std::string getDate();	

	Logger::LOG_LEVELS log_level;
	Logger *logger;
	std::string log_file;
	FILE *log_file_ptr;
};

template <typename T>
bool TestHarness::Test(T func) {
	return Test(func, true);
}

template <typename T, typename U>
bool TestHarness::Test(T func, U exp_output) {
	logger->log(Logger::HIGH, getDate(), log_file_ptr);

	bool retval = false;
	U result;
	try {
		retval = ((result = func()) == exp_output);
		logger->log(logger->LOW, ((retval) ? "Passed." : "Failed."), log_file_ptr);
		logger->log(logger->MED, " Expected output was '" + std::to_string(exp_output) + "', expression evaluated to '" + std::to_string(result) + "'", log_file_ptr);
	}
	catch (std::exception& e) {
		logger->log(logger->MED, " Error: " + std::string(e.what()), log_file_ptr);
	}
	catch (...) {
		logger->log(logger->MED, " Error: Unhandled exception has occured.", log_file_ptr);
	}	

	logger->log(logger->LOW, "\n", log_file_ptr);
	return retval;
}