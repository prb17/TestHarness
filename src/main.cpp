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
struct testObj {
	int x = 20;
	int y = 50;

	int operator()() {
		return x + y;
	}
};

int x = 40;
int y = 15;
int func1() {
	return x + y;
}

auto lambda = [=]() {
	return x - y;
};

bool iTest1() {
	return true;
}

bool iTest2() {
	return false;
}

bool iTest3() {

	throw std::range_error("Out of Range");

	return true;
}

bool iTest4() {
	int i = 0;
	int x = 5;
	int quotient = x / i; //not a standard exception so divide by 0 doesn't throw an error.  looking into it in the future
	return true;
}

bool iTest5() {

	throw std::domain_error("Out of domain scope");

	return true;
}

bool iTest6() {

	throw std::length_error("Out of length");

	return true;
}

bool iTest7() {

	throw std::overflow_error("overflow!!");

	return true;
}

bool iTest8() {

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

/////////////////////////////////////////////////////////////////////
// Test #3 - Demonstrate server with two concurrent clients
//           sending and receiving messages

//----< handler for first concurrent client >------------------------

void ThreadProcClnt1()
{
	Comm comm(EndPoint("localhost", 9891), "client1Comm");
	comm.start();
	EndPoint serverEP("localhost", 9890);
	EndPoint clientEP("localhost", 9891);
	size_t IMax = 3;
	for (size_t i = 0; i < IMax; ++i)
	{
		Message msg(serverEP, clientEP);
		msg.setName("client #1 : msg #" + Utilities::Converter<size_t>::toString(i));
		std::cout << "\n  " + comm.name() + " posting:  " << msg.getName();
		comm.postMessage(msg);
		Message rply = comm.getMessage();
		std::cout << "\n  " + comm.name() + " received: " << rply.getName();
		::Sleep(100);
	}
	::Sleep(200);
	Message stop;
	stop.setName("stop");
	stop.setDestination(serverEP);
	stop.setMsgBody("stop");
	comm.postMessage(stop);
}
//----< handler for 2nd concurrent client >--------------------------

void ThreadProcClnt2()
{
	Comm comm(EndPoint("localhost", 9892), "client2Comm");
	comm.start();
	EndPoint serverEP("localhost", 9890);
	EndPoint clientEP("localhost", 9892);
	size_t IMax = 3;
	for (size_t i = 0; i < IMax; ++i)
	{
		Message msg(serverEP, clientEP);
		msg.setName("client #2 : msg #" + Utilities::Converter<size_t>::toString(i));
		std::cout << "\n  " + comm.name() + " posting:  " << msg.getName();
		comm.postMessage(msg);
		Message rply = comm.getMessage();
		std::cout << "\n  " + comm.name() + " received: " << rply.getName();
	}
}
//----< server demonstrates two-way asynchronous communication >-----
/*
*  - One server receiving messages and sending replies to
*    two concurrent clients.
*/
void DemoClientServer()
{
	SocketSystem ss;

	EndPoint serverEP("localhost", 9890);
	EndPoint clientEP("localhost", 9891);
	Comm comm(serverEP, "serverComm");
	comm.start();
	std::thread t1(ThreadProcClnt1);
	t1.detach();
	std::thread t2(ThreadProcClnt2);
	t2.detach();

	Message msg, rply;
	rply.setName("reply");
	size_t count = 0;
	while (true)
	{
		msg = comm.getMessage();
		std::cout << "\n  " + comm.name() + " received message: " << msg.getName();
		//msg.show();
		rply.setDestination(msg.getSource());
		rply.setSource(serverEP);
		rply.setName("server reply #" + Utilities::Converter<size_t>::toString(++count) + " to " + msg.getSource().toString());
		//rply.show();
		comm.postMessage(rply);
		if (msg.getMsgBody() == "stop")
		{
			break;
		}
	}
	comm.stop();
	std::cout << "\n  press enter to quit DemoClientServer";
}

int main() {
	std::vector<std::thread> worker;
	
	testObj to = testObj();
	//TestHarness<std::function<int()>, int> intHarness = TestHarness<std::function<int()>, int>("outputfile.txt");
	SocketSystem ss;

	DemoClientServer();

	//TestHarness<std::function<int()>, int> intHarness = TestHarness<std::function<int()>, int>();
	//intHarness.startManager();

	//uint64_t test1 = intHarness.addTest(func1, 1);
	//uint64_t test2 = intHarness.addTest(to, 2);
	//uint64_t test3 = intHarness.addTest(to, 3);
	//uint64_t test4 = intHarness.addTest(to, 4);
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

