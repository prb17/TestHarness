#pragma once
#include "Logger.h"
#include "Cpp11-BlockingQueue.h"
#include "Utilities.h"
#include "Comm.h"
#include "Message.h"

#include <functional>
#include<chrono>
#include<ctime>
#include<vector>
#include<map>
#include<thread>
#include<iostream>

#define NUM_THREADS 5

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
	//TestHarness(const TestHarness<T, U>&) = delete;
	//TestHarness<T, U>& operator=(const TestHarness<T, U>&) = delete;
	TestHarness(std::string);
	TestHarness(std::string, Logger::LOG_LEVELS);	
	~TestHarness();

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
	static std::vector < std::pair<WORKER_MESSAGES, std::thread*>> thread_pool; //contains the state of the thread and thread obj itself
	static uint64_t total_test_num; //capture all tests across all loggers (of each datatype)
	static BlockingQueue<Message> readyQueue;
	static BlockingQueue<Message> testRequestQueue;

	void createThreads();
	void harnessWorker(int);
	void messageListener();
	void harnessManager();
	void workerUpdater();
	static void sendMessage(Message);
	static void processMessage(Message);
	
	uint64_t curr_test_num;	
	Logger mLogger;
	EndPoint harness_ep;
	Comm harness_comm;
	std::map<uint64_t, std::pair<T,U>> tests; //contains each test function and the expected output as well as the pair's corresponding test number in the system
	
	std::string getDate();		
	bool Test(T);
	bool Test(T, U);
	void executeSingleTest(typename std::map<uint64_t, std::pair<T, U>>::iterator);
};

template <typename T, typename U>
uint64_t TestHarness<T, U>::total_test_num = 0;

template <typename T, typename U>
std::vector<std::pair<WORKER_MESSAGES, std::thread*>> TestHarness<T, U>::thread_pool;

template <typename T, typename U>
BlockingQueue<Message> TestHarness<T, U>::readyQueue;

template <typename T, typename U>
BlockingQueue<Message> TestHarness<T, U>::testRequestQueue;

template <typename T, typename U>
void TestHarness<T, U>::createThreads() {
	std::thread listener(&TestHarness<T, U>::messageListener, this);
	std::thread manager(&TestHarness<T, U>::harnessManager, this);
	std::thread updater(&TestHarness<T, U>::workerUpdater, this);
	listener.detach();
	manager.detach();
	updater.detach();

	for (int i = 0; i < NUM_THREADS; i++) {
		std::thread child(&TestHarness<T,U>::harnessWorker, this, i);
		std::pair<WORKER_MESSAGES, std::thread*> p = std::make_pair(STARTING_UP, &child);
		thread_pool.push_back(p);
		child.detach();
	}
}

template <typename T, typename U>
void TestHarness<T, U>::harnessWorker(int worker_id) {
	mLogger.log(Logger::LOG_LEVELS::LOW, "worker: " + std::to_string(worker_id) + " is starting\n");
	int i = 0;

	while (i < 1) {
		mLogger.log(Logger::LOG_LEVELS::LOW, "worker: " + std::to_string(worker_id) + " is recieving messages\n");
		i++;
	}
	mLogger.log(Logger::LOG_LEVELS::LOW, "worker: " + std::to_string(worker_id) + " is finshed\n");
}

template <typename T, typename U>
void TestHarness<T, U>::harnessManager() {
	int i = 0;
	Message msg;
	while (i < 5) {
		msg = testRequestQueue.deQ();
		mLogger.log(Logger::LOG_LEVELS::LOW, "msg request type is: " + std::to_string(msg.getMsgType()) + "\n");
		i++;
	}
}

template <typename T, typename U>
void TestHarness<T, U>::workerUpdater() {
	int i = 0;
	Message msg;
	while (i < 5) {
		msg = readyQueue.deQ();
		mLogger.log(Logger::LOG_LEVELS::LOW, "msg request type is: " + std::to_string(msg.getMsgType()) + "\n");
		i++;
	}
}

//listens for data over a socket and enqueues the data onto appropriate queue
template <typename T, typename U>
void TestHarness<T, U>::messageListener() {
	mLogger.log(Logger::LOG_LEVELS::LOW, "starting to listen for work thread messages\n");
	
	Message msg;
	int i = 0;
	while (true) {
		msg = harness_comm.getMessage();
		if (msg.getMsgType() == Message::TEST_REQUEST) {
			testRequestQueue.enQ(msg);
		} else if (msg.getMsgType() == Message::WORKER_MESSAGE) {
			readyQueue.enQ(msg);
		} else {
			mLogger.log(Logger::LOG_LEVELS::LOW, "unhandled message type: " + std::to_string(msg.getMsgType()) + "\n");
		}
		i++;
	}
}

template <typename T, typename U>
void TestHarness<T, U>::sendMessage(Message msg) {

}

template <typename T, typename U>
TestHarness<T, U>::TestHarness() : curr_test_num(0), mLogger(Logger()) {
	mLogger.set_prefix("TestHarness: ");
	harness_ep = EndPoint("localhost", 9090);
	harness_comm = Comm(harness_ep, "master-harness");
	//harness_comm.start();
	createThreads();
}

template <typename T, typename U>
TestHarness<T, U>::TestHarness(std::string file_name) : curr_test_num(0), mLogger(file_name) {
	harness_ep = EndPoint("localhost", 9090);
	harness_comm = Comm(harness_ep, "master-harness");
	//harness_comm.start();
	createThreads();
}

template <typename T, typename U>
TestHarness<T, U>::TestHarness(std::string file_name, Logger::LOG_LEVELS level)
	: curr_test_num(0), mLogger(file_name, level) {
	harness_ep = EndPoint("localhost", 9090);
	harness_comm = Comm(harness_ep, "master-harness");
	//harness_comm.start();
	createThreads();
}

template <typename T, typename U>
TestHarness<T, U>::~TestHarness() {
	//harness_comm.stop();
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
	mLogger.log(Logger::LOG_LEVELS::MED, "\n \n \n \n");
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
	U result = false;
	
	try {
		retval = ((result = func()) == exp_output);
	}
	catch (std::exception& e) {
		mLogger.log(Logger::LOG_LEVELS::MED, "Error: " + std::string(e.what()) + "\n");
	}
	catch (...) {
		mLogger.log(Logger::LOG_LEVELS::MED, "Error: Unhandled exception has occured.\n");
	}		
	
	mLogger.log(Logger::LOG_LEVELS::LOW, "Test number #" + std::to_string(curr_test_num) + ": " + ((retval) ? "Passed.\n" : "Failed.\n"));
	mLogger.log(Logger::LOG_LEVELS::MED, "Expected output was '" + std::to_string(exp_output) + "', expression evaluated to '" + std::to_string(result) + "'\n");

	return retval;
}