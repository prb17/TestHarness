#pragma once
#include "Logger.h"
#include <functional>
#include<chrono>
#include<ctime>

class TestHarness {
public:
	TestHarness();
	TestHarness(std::string);

	template <typename T, typename U>
	bool Test(Logger::LOG_LEVELS level, T func, U exp_output);

private:
	bool openLogFile();	

	Logger::LOG_LEVELS log_level;
	Logger *logger;
	std::string log_file;
	FILE *log_file_ptr;
};

template <typename T, typename U>
bool TestHarness::Test(Logger::LOG_LEVELS level, T func, U exp_output) {
	bool retval = false;
	std::string error_msg = "";
	U result;
	std::string log_msg = "TestHarness:Test:";
	try {
		retval = ((result = func()) == exp_output);
	}
	catch (std::exception& e) {
		error_msg = error_msg + "error: " + e.what();
	}
	catch (...) {

	}

	switch (level) {
		case logger->HIGH: {
			const int size = 26;
			char buf[size];
			auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			ctime_s(buf, size, &timenow);
			std::string date = std::string(buf, size);
			date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());
			log_msg = date + log_msg;
		}		
		case logger->MED: {
			if (error_msg == "") {
				log_msg = log_msg + " " + "expected output was-" + std::to_string(exp_output) + ", expression evaluated to-" + std::to_string(result);
			}
			else {
				log_msg = log_msg + " " + error_msg;
			}
		}
		case logger->LOW: {
			log_msg = log_msg + "-" + ((retval) ? "passed" : "failed");
		}
	}

	log_msg = log_msg + '\n';
	logger->log(level, log_msg, log_file_ptr);

	return retval;
}