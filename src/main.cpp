#include "TestHarness.h"
#include <iostream>
#include <functional>
#include <cassert>
#include <thread>

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

int main() {
	std::cout << "hello world" << std::endl;

	bool result;
	testObj to = testObj();
	TestHarness harness = TestHarness("outputfile.txt");
	result = harness.Test<testObj, int>(Logger::LOW, to, 25);	
	result = harness.Test<testObj, int>(Logger::MED, to, 55);
	result = harness.Test<testObj, int>(Logger::HIGH, to, 70);
	result = harness.Test<testObj, double>(Logger::HIGH, to, 70.01);
	std::cout << std::endl;

	result = harness.Test<int(), int>(Logger::LOW, func1, 25);
	result = harness.Test<int(), int>(Logger::MED, func1, 55);
	result = harness.Test<int(), int>(Logger::HIGH, func1, 70);
	result = harness.Test<int(), double>(Logger::HIGH, func1, 70.01);
	std::cout << std::endl;

	std::function<int()> func2 = lambda;
	result = harness.Test<std::function<int()>, int>(Logger::LOW, func2, 25);
	result = harness.Test<std::function<int()>, int>(Logger::MED, func2, 55);
	result = harness.Test<std::function<int()>, int>(Logger::HIGH, func2, 70);
	result = harness.Test<std::function<int()>, double>(Logger::HIGH, func2, 70.01);
	std::cout << std::endl;
}