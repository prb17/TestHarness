#pragma once

#include <thread>

struct testMetaData {
	std::string test_name;
};

struct testObj {
	int x = 20;
	int y = 50;
    std::string test_results = "test results: \n";

	void operator()() {
		test_results += test1() ? "test1 passed\n" : "test1 failed\n";
        test_results += test2() ? "test2 passed\n" : "test2 failed\n";
    }

    std::string getResults() {
        return test_results;
    }

    bool test1() {
        return x == y;
    }
    bool test2() {
        return x == y - 30;
    }
};

int x = 40;
int y = 15;
int func1() {
	std::this_thread::sleep_for(std::chrono::seconds(1));
	return x + y;
}

auto lambda = []() {
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return x - y;
};

bool iTest1() {
	for (uint64_t i = 0; i < 5; i++)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
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