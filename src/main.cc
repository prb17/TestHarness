#include<iostream>
#include "TestHarness.h"

#include <stdexcept>
#include <functional>
#include <cassert>
#include <thread>
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include "SimpleLogger.h"
#include "tests.h"

// void prev_tests() {
// 	TestHarness<std::function<int()>, int> intHarness = TestHarness<std::function<int()>, int>();
// 	TestHarness<std::function<void()>, bool> boolHarness = TestHarness<std::function<void()>, bool>();
// 	std::vector<uint64_t> test_list;
// 	testObj to = testObj();

// 	test_list.push_back(intHarness.addTest(func1, 55));
// 	test_list.push_back(intHarness.addTest(iTest1, true));

// 	boolHarness.addTest(to, true);
// 	// test_list.push_back(intHarness.addTest(iTest2, true));
// 	// test_list.push_back(intHarness.addTest(iTest3, true));
// 	// test_list.push_back(intHarness.addTest(iTest4, true));
// 	// test_list.push_back(intHarness.addTest(iTest5, true));
// 	// test_list.push_back(intHarness.addTest(iTest6, true));
// 	// test_list.push_back(intHarness.addTest(iTest7, true));
// 	// test_list.push_back(intHarness.addTest(iTest8, true));

// 	intHarness.executeSingleTest(test_list[2]);
// 	intHarness.removeTest(test_list[3]);

// 	uint64_t num = intHarness.getNumTests();
// 	intHarness.removeTest(test_list[2]);
// 	intHarness.executeSingleTest(test_list[4]);
// 	//intHarness.clearTests();

// 	intHarness.removeTest(test_list[5]);
// 	uint64_t test7 = intHarness.addTest(lambda, 7);
// 	//intHarness.setIgnoreLevel(LOG_LEVELS::INFO);
// 	intHarness.executeTests();
// 	intHarness.setIgnoreLevel(LOG_LEVELS::WARN);
// 	//intHarness.executeTests();
// 	intHarness.clearTests();

// }

int main() {
	//prev_tests();	

	TestHarness<std::function<int()>, int> intHarness = TestHarness<std::function<int()>, int>();
	intHarness.test_listener();
}