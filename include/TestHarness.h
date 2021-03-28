#pragma once
#include "SimpleLogger.h"
#include "queues/BlockingQueue.h"
#include "ThreadPool.h"

#include<functional>
#include<chrono>
#include<ctime>
#include<vector>
#include<map>
#include<thread>
#include<iostream>

#include "zmq.h"
#include "czmq.h"

#include "tests.h"

#define NUM_THREADS 20

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
	~TestHarness();

	void setIgnoreLevel(LOG_LEVELS);

	//sync behavior
	uint64_t getNumTests();
	uint64_t addTest(T);
	uint64_t addTest(T, U);
	void removeTest(uint64_t);
	void clearTests();
	void executeSingleTest(uint64_t);
	void executeTests();

	//async behavior
	zloop_t *loop;
	zsock_t *socket;
	void test_listener();

private:
	static uint64_t total_test_num; //capture all tests across all loggers (of each different T,U pair)	
	static int setup_test(zloop_t *, zsock_t *, void *);

	uint64_t curr_test_num;
	SimpleLogger logger;
	std::map<uint64_t, std::pair<T, U>> tests; //contains each test function and the expected output as well as the pair's corresponding test number in the system
	ThreadPool<std::function<bool()>> threadPool;

	std::string getDate();
	bool Test(T);
	bool Test(T, U);
	void executeSingleTest(typename std::map<uint64_t, std::pair<T, U>>::iterator);
	void executeSingleTestAsync(uint64_t);
};

template <typename T, typename U>
uint64_t TestHarness<T, U>::total_test_num = 0;

template <typename T, typename U>
TestHarness<T, U>::TestHarness() : curr_test_num(0), logger(SimpleLogger()), 
		threadPool(NUM_THREADS) {
			loop = zloop_new();
			socket = zsock_new_sub ("@tcp://*:9281", "");
			zloop_reader(loop, socket, setup_test, nullptr);
}

template <typename T, typename U>
TestHarness<T, U>::TestHarness(std::string file_name) : curr_test_num(0), 
		logger(file_name), threadPool(NUM_THREADS) {
			loop = zloop_new();
			socket = zsock_new_sub ("@tcp://*:9281", "");
			zloop_reader(loop, socket, setup_test, nullptr);
}

template <typename T, typename U>
void TestHarness<T, U>::test_listener() {
	std::cout << "starting zloop" << std::endl;
	zloop_start(loop);
}

template<typename T, typename U>
int TestHarness<T, U>::setup_test(zloop_t *loop, zsock_t *socket, void *pb) {
	std::cout << "reading a test message!" << std::endl;
	zmsg_t *msg = zmsg_recv(socket);
	std::string str = "msg header was: " + std::string(zmsg_popstr(msg));
	std::cout << str << std::endl;
	std::string testObjContent = std::string(zmsg_popstr(msg));
	str = "msg content was: " + testObjContent;
	std::cout << str << std::endl;
	testObj to;
	memcpy(&to, &testObjContent, 8);
	to();
	std::cout << to.getResults() << std::endl;
	return 0;
}

template <typename T, typename U>
TestHarness<T, U>::~TestHarness() {
	zloop_reader_end(loop, socket);
	zloop_destroy(&loop);
}

template <typename T, typename U>
void TestHarness<T, U>::setIgnoreLevel(LOG_LEVELS level) {
	logger.set_ignore_level(level);
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
	bool result;
	logger.info("Running test number #" + std::to_string(curr_test_num));
	if (it != tests.end()) {
		result = Test(it->second.first, it->second.second);
	} else {
		logger.debug("Can't run test number '" + std::to_string(curr_test_num));
		logger.debug("The test number '" + std::to_string(curr_test_num) + "' most likely doesn't exist.");
	}
	logger.info("test number #" + std::to_string(curr_test_num) + " completed");
	logger.log(LOG_LEVELS::INFO, "\n");
}

template <typename T, typename U>
void TestHarness<T, U>::executeSingleTestAsync(uint64_t test_num) {
	logger.info("attempting to read dll library");
	threadPool.doJob( [=]() {
			bool retval;
			
			logger.info("Running test:" + std::to_string(test_num));
			retval = Test(tests.find(test_num).first, true);
			logger.info("test :" + std::to_string(test_num) + " completed");

			return retval; 
		}
	);	
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
		logger.error("Error: " + std::string(e.what()));
	}
	catch (...) {
		logger.error("Error: Unhandled exception has occured.");
	}		
	
	logger.info("Test number #" + std::to_string(curr_test_num) + ": " + ((retval) ? "Passed." : "Failed."));
	logger.debug("Expected output was '" + std::to_string(exp_output) + "', expression evaluated to '" + std::to_string(result) + "'");

	return retval;
}