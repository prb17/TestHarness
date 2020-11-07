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

bool ITest() {
	return false;
}

bool ITest1()
{
	return true;
}

bool ITest2()
{
	return false;
}
bool ITest3()
{
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

