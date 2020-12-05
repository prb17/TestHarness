#include<iostream>
#include "../include/TestHarness.h"

#include "../include/TestHarness.h"
#include <stdexcept>
#include <functional>
#include <cassert>
#include <thread>
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock
#include <deque>
#include <winSock2.h>

bool termpool;
std::mutex queue_mutex;
std::condition_variable condition;
std::deque<void (*)()> Queue;

Logger mainLogger;

struct testObj {
	int x = 20;
	int y = 50;

	int operator()() {
		std::this_thread::sleep_for(std::chrono::seconds(2));
		return x + y;
	}
};

int x = 40;
int y = 15;
int func1() {
	std::this_thread::sleep_for(std::chrono::seconds(1));
	return x + y;
}

auto lambda = [=]() {
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return x - y;
};

bool iTest1() {
	for (uint64_t i = 0; i < 5000; i++)
	{
		Sleep(1);
	}
	return true;
}

bool iTest2() {
	std::this_thread::sleep_for(std::chrono::seconds(10));
	return false;
}

bool iTest3() {
	std::this_thread::sleep_for(std::chrono::seconds(5));
	throw std::range_error("Out of Range");
	return true;
}

bool iTest4() {
	int i = 0;
	int x = 5;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	//int quotient = x / i; //not a standard exception so divide by 0 doesn't throw an error.  looking into it in the future
	return true;
}

bool iTest5() {
	std::this_thread::sleep_for(std::chrono::seconds(1));
	throw std::domain_error("Out of domain scope");
	return true;
}

bool iTest6() {
	std::this_thread::sleep_for(std::chrono::seconds(2));
	throw std::length_error("Out of length");
	return true;
}

bool iTest7() {
	std::this_thread::sleep_for(std::chrono::seconds(6));
	throw std::overflow_error("overflow!!");	
	return true;
}

bool iTest8() {
	std::this_thread::sleep_for(std::chrono::seconds(10));
	throw std::underflow_error("Underflow!!");	
	return true;
}

void Infinite_loop_function()
{
	void (*Job)();
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			condition.wait(lock, [] {return !Queue.empty() || termpool; });
			Job = Queue.front();
			Queue.pop_front();
		}
		Job(); // function<void()> type
	}
};

void Add_Job(void (*New_Job)())
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		Queue.push_back(New_Job);
	}
	condition.notify_one();
};

void harnessProc1(EndPoint dest, std::vector<uint64_t>* tests, TestHarness<std::function<int()>, int>* harness) {
	EndPoint client_ep("localhost", 10050);
	int i, j;
	std::string clientName = "client 1";
	Comm client_comm(client_ep, clientName);
	client_comm.start();
	Message msg(dest, client_ep);
	msg.setName(clientName);
	msg.setAuthor(clientName);
	msg.setMsgType(Message::TEST_REQUEST);

	Message rply;
	for (i = 0, j = 0; i < tests->size() * 2; j++, i++) {
		if (j >= tests->size())
			j = 0; //loop through list twice to send a ton of test requests. 
		msg.setMsgBody(std::to_string(tests->at(j)));
		client_comm.postMessage(msg);
		mainLogger.log(Logger::LOG_LEVELS::LOW, "sent test request '" + std::to_string(tests->at(j)) + "' to test harness", clientName + ": ");
	}
	while (i > 0)
	{
		rply = client_comm.getMessage();
		mainLogger.log(Logger::LOG_LEVELS::LOW, "received reply from test harness, " + rply.getName() + " result: " + rply.getMsgBody(), clientName + ": ");
		i--;
	}
	mainLogger.log(Logger::LOG_LEVELS::LOW, "ALL TESTS COMPLETED EXITING for '" + clientName + "'");
	//msg.setName("quit");
	//client_comm.postMessage(msg);
	//harness->stop();
}

void harnessProc2(EndPoint dest, std::vector<uint64_t>* tests, TestHarness<std::function<int()>, int>* harness) {
	EndPoint client_ep("localhost", 10055);
	int i, j;
	std::string clientName = "client 2";
	Comm client_comm(client_ep, clientName);
	client_comm.start();
	Message msg(dest, client_ep);
	msg.setName(clientName);
	msg.setAuthor(clientName);
	msg.setMsgType(Message::TEST_REQUEST);

	Message rply;
	for (i = 0, j = 0; i < tests->size() * 2; j++, i++) {
		if (j >= tests->size())
			j = 0; //loop through list twice to send a ton of test requests. 
		msg.setMsgBody(std::to_string(tests->at(j)));
		client_comm.postMessage(msg);
		mainLogger.log(Logger::LOG_LEVELS::LOW, "sent test request '" + std::to_string(tests->at(j)) + "' to test harness", clientName + ": ");
	}
	while (i > 0)
	{
		rply = client_comm.getMessage();
		mainLogger.log(Logger::LOG_LEVELS::LOW, "received reply from test harness, " + rply.getName() + " result: " + rply.getMsgBody() + " test finished at: " + rply.getDate(), clientName + ": ");
		i--;
	}
	mainLogger.log(Logger::LOG_LEVELS::LOW, "ALL TESTS COMPLETED EXITING for '" + clientName + "'");
	/*msg.setName("quit");
	client_comm.postMessage(msg);*/
	harness->stop(); //assuming this will get called after client 1
}

int main() {
	mainLogger.set_prefix("main: ");
	SocketSystem ss;
	std::cout << "hello world" << std::endl;
	TestHarness<std::function<int()>, int> intHarness = TestHarness<std::function<int()>, int>();

	std::vector<uint64_t> test_list;
	testObj to = testObj();

	test_list.push_back(intHarness.addTest(func1, 55));
	test_list.push_back(intHarness.addTest(to, 70));
	test_list.push_back(intHarness.addTest(iTest1, true));
	test_list.push_back(intHarness.addTest(to, 4));
	test_list.push_back(intHarness.addTest(to, 4));
	test_list.push_back(intHarness.addTest(iTest2, true));
	test_list.push_back(intHarness.addTest(iTest3, true));
	test_list.push_back(intHarness.addTest(iTest4, true));
	test_list.push_back(intHarness.addTest(iTest5, true));
	test_list.push_back(intHarness.addTest(iTest6, true));
	test_list.push_back(intHarness.addTest(iTest7, true));
	test_list.push_back(intHarness.addTest(iTest8, true));

	std::thread h1(harnessProc1, intHarness.getHarnessEndpoint(), &test_list, &intHarness);
	h1.detach();

	std::thread h2(harnessProc2, intHarness.getHarnessEndpoint(), &test_list, &intHarness);
	h2.detach();

	intHarness.startManager();
	mainLogger.log(Logger::LOG_LEVELS::LOW, "Shutting down main");
	//intHarness.executeSingleTest(test2);
	//intHarness.removeTest(test3);

	//uint64_t num = intHarness.getNumTests();
	//intHarness.removeTest(test2);
	//intHarness.executeSingleTest(test4);
	//intHarness.clearTests();

	//uint64_t test5 = intHarness.addTest(to, 5);
	//uint64_t test6 = intHarness.addTest(to, 6);
	//intHarness.removeTest(test5);
	//uint64_t test7 = intHarness.addTest(lambda, 7);
	//intHarness.setLoggerLevel(Logger::LOG_LEVELS::MED);
	//intHarness.executeTests();
	//intHarness.setLoggerLevel(Logger::LOG_LEVELS::HIGH);
	//intHarness.executeTests();
	//intHarness.clearTests();

	//TestHarness<std::function<bool()>, bool> boolHarness = TestHarness<std::function<bool()>, bool>();
	//test1 = boolHarness.addTest(iTest1);
	//test2 = boolHarness.addTest(iTest2);
	//test3 = boolHarness.addTest(iTest3);
	//boolHarness.executeTests();
	//boolHarness.setLoggerLevel(Logger::LOG_LEVELS::HIGH);

	//boolHarness.removeTest(test2);
	//boolHarness.executeTests();
}