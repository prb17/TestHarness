#include "../include/TestHarness.h"
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
	int quotient = x / i;
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


int main() {
	testObj to = testObj();
	//TestHarness<std::function<int()>, int> intHarness = TestHarness<std::function<int()>, int>("outputfile.txt");
	TestHarness<std::function<int()>, int> intHarness = TestHarness<std::function<int()>, int>();

	int test1 = intHarness.addTest(func1, 1);
	int test2 = intHarness.addTest(to, 2);
	int test3 = intHarness.addTest(to, 3);
	int test4 = intHarness.addTest(to, 4);
	intHarness.executeSingleTest(test2);
	intHarness.removeTest(test3);

	int num = intHarness.getNumTests();
	intHarness.removeTest(test2);
	intHarness.executeSingleTest(test4);
	intHarness.clearTests();

	int test5 = intHarness.addTest(to, 5);
	int test6 = intHarness.addTest(to, 6);
	intHarness.removeTest(test5);
	int test7 = intHarness.addTest(to, 7);

	intHarness.executeTests();

	TestHarness<std::function<bool()>, bool> boolHarness = TestHarness<std::function<bool()>, bool>();
	test1 = boolHarness.addTest(ITest1);
	test2 = boolHarness.addTest(ITest2);
	test3 = boolHarness.addTest(ITest3);
	boolHarness.executeTests();

	boolHarness.removeTest(test2);
	boolHarness.executeTests();
}

