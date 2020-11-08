#pragma once
#include "Logger.h"
#include <functional>
#include<chrono>
#include<ctime>
#include<vector>
#include<map>

template <typename T, typename U>
class TestHarness {
public:
	TestHarness();
	TestHarness(std::string);
	TestHarness(std::string, Logger::LOG_LEVELS);	

	void setLoggerLevel(Logger::LOG_LEVELS);
	Logger::LOG_LEVELS getLoggerLevel();

	uint64_t getNumTests();
	uint64_t addTest(T);
	uint64_t addTest(T, U);
	void removeTest(uint64_t);
	void clearTests();
	void executeSingleTest(uint64_t);	
	void executeTests();	

private:	
	static uint64_t total_test_num;
	uint64_t curr_test_num;
	
	//bool openLogFile();		
	std::string getDate();		
	bool Test(T);
	bool Test(T, U);
	void executeSingleTest(typename std::map<uint64_t, std::pair<T, U>>::iterator);
	
	Logger mLogger;
	std::map<uint64_t, std::pair<T,U>> tests; //contains each test function and the expected output as well as the pair's corresponding test number in the system
};

template <typename T, typename U>
uint64_t TestHarness<T, U>::total_test_num = 0;

template <typename T, typename U>
TestHarness<T, U>::TestHarness() : curr_test_num(0), mLogger("test_output") {
}

template <typename T, typename U>
TestHarness<T, U>::TestHarness(std::string file_name) : curr_test_num(0), mLogger(file_name) {
	
	//openLogFile();
	curr_test_num = 0;
}

template <typename T, typename U>
TestHarness<T, U>::TestHarness(std::string file_name, Logger::LOG_LEVELS level)
	: curr_test_num(0) {
	mLogger(file_name, level);
	//openLogFile();
}

/*template <typename T, typename U>
bool TestHarness<T, U>::openLogFile() {
	errno_t err;
	err = fopen_s(&log_file_ptr, log_file.c_str(), "a");
	if (err != 0) {
		logger->log(logger->HIGH, "Could not create handle for file i/o for file: '" + log_file + "' error code was: '" + std::to_string(err) + "'", stdout);
	}

	return err;
}*/

template <typename T, typename U>
std::string TestHarness<T, U>::getDate() {
	//This block is for the current time only, I couldn't find anything more concise not to say there isn't a better way
	const int size = 26;
	char buf[size];
	auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	ctime_s(buf, size, &timenow);
	std::string date = std::string(buf, size);
	date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());
	return "[" + date + "]";
}

template <typename T, typename U>
void TestHarness<T, U>::setLoggerLevel(Logger::LOG_LEVELS level) {
	mLogger->set_level(level);
}

template <typename T, typename U>
Logger::LOG_LEVELS TestHarness<T, U>::getLoggerLevel() {
	return mLogger->get_level();
}

template <typename T, typename U>
uint64_t TestHarness<T, U>::getNumTests() {
	return total_test_num;
}

template <typename T, typename U>
void TestHarness<T, U>::removeTest(uint64_t idx) {
	tests.erase(idx);
}

template <typename T, typename U>
void TestHarness<T, U>::clearTests() {
	tests.clear();
	//total_test_num = 0;
	curr_test_num = 0;
}

template <typename T, typename U>
uint64_t TestHarness<T, U>::addTest(T testFunc) {
	return addTest(testFunc, 1);
}

template <typename T, typename U>
uint64_t TestHarness<T, U>::addTest(T testFunc, U exp_output) {
	tests.insert(std::pair< uint64_t, std::pair<T,U> >(total_test_num, std::pair<T,U>(testFunc, exp_output)));
	return total_test_num++; //return current value of total test num, then increment total_test_num by 1
}

template <typename T, typename U>
void TestHarness<T, U>::executeSingleTest(uint64_t test) {
	executeSingleTest(tests.find(test));
}

template <typename T, typename U>
void TestHarness<T, U>::executeSingleTest(typename std::map<uint64_t, std::pair<T, U>>::iterator it) {
	curr_test_num = it->first + 1;

	mLogger.log(Logger::LOG_LEVELS::HIGH, "Running test number #" + std::to_string(curr_test_num) + " starting time: " + getDate() + "\n");
	if (it != tests.end()) {
		Test(it->second.first, it->second.second);
	} else {
		mLogger.log(Logger::LOG_LEVELS::MED, "Can't run test number '" + std::to_string(curr_test_num) + "'\n");
		mLogger.log(Logger::LOG_LEVELS::MED, "The test number '" + std::to_string(curr_test_num) + "' most likely doesn't exist.\n");
	}
	mLogger.log(Logger::LOG_LEVELS::HIGH, "test number #" + std::to_string(curr_test_num) + " completed at time: " + getDate());
	mLogger.log(Logger::LOG_LEVELS::LOW, "\n \n \n \n");
}

template <typename T, typename U>
void TestHarness<T, U>::executeTests() {
	typename std::map<uint64_t, std::pair<T, U>>::iterator it;
	for (it = tests.begin(); it != tests.end(); it++) {
		executeSingleTest(it);
	}
}

template <typename T, typename U>
bool TestHarness<T, U>::Test(T func) {
	return Test(func, true);
}

template <typename T, typename U>
bool TestHarness<T, U>::Test(T func, U exp_output) {
	bool retval = false;
	U result;
	
	try {
		retval = ((result = func()) == exp_output);
		mLogger.log(Logger::LOG_LEVELS::LOW, ((retval) ? "Passed.\n" : "Failed.\n"));
		mLogger.log(Logger::LOG_LEVELS::MED, "Expected output was '" + std::to_string((U)exp_output) + "', expression evaluated to '" + std::to_string(result) + "'\n");
	}
	catch (std::exception& e) {
		mLogger.log(Logger::LOG_LEVELS::MED, "Error: " + std::string(e.what()) + "\n");
	}
	catch (...) {
		mLogger.log(Logger::LOG_LEVELS::MED, "Error: Unhandled exception has occured.\n");
	}		
	
	return retval;
}