#pragma once
#include "Logger.h"
#include "Cpp11-BlockingQueue.h"
#include "Utilities.h"
#include "Comm.h"
#include "Message.h"
#include "ThreadPool.h"

#include <functional>
#include<chrono>
#include<ctime>
#include<vector>
#include<map>
#include<thread>
#include<iostream>

#define NUM_THREADS 20

using namespace MsgPassingCommunication;
using namespace Sockets;

enum WORKER_MESSAGES {
	STARTING_UP,
	READY,
	PROCESSING
};

template <typename T, typename U>
class TestHarness {
public:
	TestHarness();
	TestHarness(std::string);
	TestHarness(std::string, Logger::LOG_LEVELS);
	~TestHarness();

	void setLoggerLevel(Logger::LOG_LEVELS);
	Logger::LOG_LEVELS getLoggerLevel();

	uint64_t getNumTests();
	EndPoint getHarnessEndpoint();
	uint64_t addTest(T);
	uint64_t addTest(T, U);
	void removeTest(uint64_t);
	void clearTests();
	void executeSingleTest(uint64_t);
	void executeTests();
	void startManager();
	void stop();

private:
	static uint64_t total_test_num; //capture all tests across all loggers (of each different T,U pair)	

	uint64_t curr_test_num;
	Logger mLogger;
	EndPoint harness_ep;
	Comm harness_comm;
	size_t basePort;
	std::map<uint64_t, std::pair<T, U>> tests; //contains each test function and the expected output as well as the pair's corresponding test number in the system
	ThreadPool<std::function<bool()>> threadPool;

	void messageListener();
	std::string getDate();
	bool Test(T);
	bool Test(T, U);
	void executeSingleTest(typename std::map<uint64_t, std::pair<T, U>>::iterator);
	void executeSingleTestAsync(Message);
};

template <typename T, typename U>
uint64_t TestHarness<T, U>::total_test_num = 0;

template <typename T, typename U>
TestHarness<T, U>::TestHarness() : curr_test_num(0), basePort(9090), mLogger(Logger()), 
		harness_ep(EndPoint("localhost", 9090)), harness_comm(harness_ep, "master-harness"), //TODO: basePort won't work when creating EndPoint for some reason
		threadPool(NUM_THREADS) { 
	mLogger.set_prefix("TestHarness: ");
	harness_comm.start();
}

template <typename T, typename U>
TestHarness<T, U>::TestHarness(std::string file_name) : curr_test_num(0), mLogger(file_name),
		harness_ep(EndPoint("localhost", 9090)), harness_comm(harness_ep, "master-harness"), threadPool(NUM_THREADS) {
	mLogger.set_prefix("TestHarness: ");
	harness_comm.start();
}

template <typename T, typename U>
TestHarness<T, U>::TestHarness(std::string file_name, Logger::LOG_LEVELS level)
		: curr_test_num(0), mLogger(file_name, level), harness_ep(EndPoint("localhost", 9090)), 
		harness_comm(harness_ep, "master-harness"), threadPool(NUM_THREADS) {
	mLogger.set_prefix("TestHarness: ");
	harness_comm.start();
}

template <typename T, typename U>
TestHarness<T, U>::~TestHarness() {}

//listens for data over a socket and enqueues the data onto appropriate queue
template <typename T, typename U>
void TestHarness<T, U>::messageListener() {
	mLogger.log(Logger::LOG_LEVELS::LOW, "starting to listen for worker and test request messages", "Message Listener: ");

	Message msg;
	while (true) {
		mLogger.log(Logger::LOG_LEVELS::LOW, "looking for new messages", "MessageListener: ");
		msg = harness_comm.getMessage();
		if (msg.getName() == "quit") {
			mLogger.log(Logger::LOG_LEVELS::LOW, "received 'quit', quitting.", "MessageListener: ");
			break;
		}
		mLogger.log(Logger::LOG_LEVELS::LOW, "msg received from '" + msg.getAuthor() + "'", "MessageListener: ");
		if (msg.getMsgType() == Message::TEST_REQUEST) {
			executeSingleTestAsync(msg);
		}
		else {
			mLogger.log(Logger::LOG_LEVELS::LOW, "unhandled message type: " + std::to_string(msg.getMsgType()), "MessageListener: ");
		}
	}
}

template <typename T, typename U>
void TestHarness<T, U>::startManager() {
	std::thread listener(&TestHarness<T, U>::messageListener, this);
	listener.join();

}

template <typename T, typename U>
void TestHarness<T, U>::stop() {
	threadPool.~ThreadPool();
	Message msg = Message(harness_ep, harness_ep);
	msg.setName("quit");
	harness_comm.postMessage(msg);
}

template <typename T, typename U>
std::string TestHarness<T, U>::getDate() {
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
	mLogger.set_level(level);
}

template <typename T, typename U>
Logger::LOG_LEVELS TestHarness<T, U>::getLoggerLevel() {
	return mLogger.get_level();
}

template <typename T, typename U>
uint64_t TestHarness<T, U>::getNumTests() {
	return total_test_num;
}

template <typename T, typename U>
EndPoint TestHarness<T, U>::getHarnessEndpoint() {
	return harness_ep;
}

template <typename T, typename U>
void TestHarness<T, U>::removeTest(uint64_t idx) {
	tests.erase(idx);
}

template <typename T, typename U>
void TestHarness<T, U>::clearTests() {
	tests.clear();
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
void TestHarness<T, U>::executeTests() {
	typename std::map<uint64_t, std::pair<T, U>>::iterator it;
	for (it = tests.begin(); it != tests.end(); it++) {
		executeSingleTest(it);
	}
}

template <typename T, typename U>
void TestHarness<T, U>::executeSingleTest(uint64_t test) {
	executeSingleTest(tests.find(test));
}

template <typename T, typename U>
void TestHarness<T, U>::executeSingleTest(typename std::map<uint64_t, std::pair<T, U>>::iterator it) {
	curr_test_num = it->first + 1;

	mLogger.log(Logger::LOG_LEVELS::HIGH, "Running test number #" + std::to_string(curr_test_num) + " starting time: " + getDate());
	if (it != tests.end()) {
		Test(it->second.first, it->second.second);
	} else {
		mLogger.log(Logger::LOG_LEVELS::MED, "Can't run test number '" + std::to_string(curr_test_num) + "'\n");
		mLogger.log(Logger::LOG_LEVELS::MED, "The test number '" + std::to_string(curr_test_num) + "' most likely doesn't exist.");
	}
	mLogger.log(Logger::LOG_LEVELS::HIGH, "test number #" + std::to_string(curr_test_num) + " completed at time: " + getDate());
	mLogger.log(Logger::LOG_LEVELS::MED, "\n \n \n \n");
}

template <typename T, typename U>
void TestHarness<T, U>::executeSingleTestAsync(Message request_msg) {
	uint64_t local_test_num = std::stoi(request_msg.msg_body);
	typename std::map<uint64_t, std::pair<T, U>>::iterator it = tests.find(local_test_num);
	curr_test_num = it->first + 1;
	
	if (it != tests.end()) {
		threadPool.doJob( [=]() {
				mLogger.log(Logger::LOG_LEVELS::HIGH, "Running test number #" + std::to_string(local_test_num) + " starting time: " + getDate());
				bool retval = Test(it->second.first, it->second.second);

				//setup up message to go to whoever requested it, and from the harness
				Message result_msg = Message(request_msg.source, harness_ep);
				result_msg.setName("test '" + std::to_string(local_test_num) + "'");
				result_msg.setDate(getDate());
				//construct result
				mLogger.log(Logger::LOG_LEVELS::LOW, "sending TEST: '" + std::to_string(local_test_num) + "' result back to requester");
				if (retval)
					result_msg.msg_body = "PASSED";
				else
					result_msg.msg_body = "FAILED";
				harness_comm.postMessage(result_msg);
				mLogger.log(Logger::LOG_LEVELS::MED, "Finished with test '" + std::to_string(local_test_num) + "'\n \n \n \n");

				return retval; 
			}
		);
	}
	else {
		mLogger.log(Logger::LOG_LEVELS::MED, "Can't run test number '" + std::to_string(curr_test_num) + "'\n");
		mLogger.log(Logger::LOG_LEVELS::MED, "The test number '" + std::to_string(curr_test_num) + "' most likely doesn't exist.");
	}	
}

template <typename T, typename U>
bool TestHarness<T, U>::Test(T func) {
	return Test(func, true);
}

template <typename T, typename U>
bool TestHarness<T, U>::Test(T func, U exp_output) {
	bool retval = false;
	U result = false;
	
	try {
		retval = ((result = func()) == exp_output);
	}
	catch (std::exception& e) {
		mLogger.log(Logger::LOG_LEVELS::MED, "Error: " + std::string(e.what()));
	}
	catch (...) {
		mLogger.log(Logger::LOG_LEVELS::MED, "Error: Unhandled exception has occured.");
	}		
	
	mLogger.log(Logger::LOG_LEVELS::LOW, "Test number #" + std::to_string(curr_test_num) + ": " + ((retval) ? "Passed." : "Failed."));
	mLogger.log(Logger::LOG_LEVELS::MED, "Expected output was '" + std::to_string(exp_output) + "', expression evaluated to '" + std::to_string(result) + "'");

	return retval;
}