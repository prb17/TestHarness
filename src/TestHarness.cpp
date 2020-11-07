#include "../include/TestHarness.h"
#include <string>
#include <cassert>

TestHarness::TestHarness() {
	logger = logger->Instance();
	log_file_ptr = stdout;
}

TestHarness::TestHarness(std::string file_name) : log_file(file_name) {
	logger = logger->Instance();
	openLogFile();
}

TestHarness::TestHarness(std::string file_name, Logger::LOG_LEVELS level) 
	: log_file(file_name), log_level(level) {
	logger = logger->Instance();
	openLogFile();
}

bool TestHarness::openLogFile() {
	errno_t err;
	err = fopen_s(&log_file_ptr, log_file.c_str(), "a");
	if (err != 0) {
		logger->log(logger->HIGH, "Could not create handle for file i/o for file: '" + log_file + "' error code was: '" + std::to_string(err) + "'", stdout);
	}

	return err;
}


std::string TestHarness::getDate() {
	//This block is for the current time only, I couldn't find anything more concise not to say there isn't a better way
	const int size = 26;
	char buf[size];
	auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	ctime_s(buf, size, &timenow);
	std::string date = std::string(buf, size);
	date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());
	return date;
}

void TestHarness::setLoggerLevel(Logger::LOG_LEVELS level) {
	log_level = level;
}

Logger::LOG_LEVELS TestHarness::getLoggerLevel() {
	return log_level;
}