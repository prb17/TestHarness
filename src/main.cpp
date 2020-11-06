#include "../include/TestHarness.h"
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

bool ITest() {
	int x = 5;
	int y = 7;
	return x == y;
}

int main() {
	std::cout << "hello world" << std::endl;
	bool result;
	testObj to = testObj();
	//TestHarness harness = TestHarness("outputfile.txt");
	TestHarness harness = TestHarness();
	harness.Test(ITest);

	result = harness.Test<testObj, int>(to, 25);
	result = harness.Test<testObj, int>(to, 55);
	result = harness.Test<testObj, int>(to, 70);
	result = harness.Test<testObj, double>(to, 70.01);

	result = harness.Test<int(), int>(func1, 25);
	result = harness.Test<int(), int>(func1, 55);
	result = harness.Test<int(), int>(func1, 70);
	result = harness.Test<int(), double>(func1, 70.01);

	std::function<int()> func2 = lambda;
	result = harness.Test<std::function<int()>, int>(func2, 25);
	result = harness.Test<std::function<int()>, int>(func2, 55);
	result = harness.Test<std::function<int()>, int>(func2, 70);
	result = harness.Test<std::function<int()>, double>(func2, 70.01);

	
}