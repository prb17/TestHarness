// MathLibrary.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <utility>
#include <limits.h>
#include<iostream>
#include <thread>
#include "ITestLibrary.h"

// DLL internal state variables:
static unsigned long long previous_;  // Previous value, if any
static unsigned long long current_;   // Current sequence value
static unsigned index_;               // Current seq. position

// Initialize a Fibonacci relation sequence
// such that F(0) = a, F(1) = b.
// This function must be called before any other function.
void fibonacci_init(
    const unsigned long long a,
    const unsigned long long b)
{
    index_ = 0;
    current_ = a;
    previous_ = b; // see special case when initialized
}

// Produce the next value in the sequence.
// Returns true on success, false on overflow.
bool fibonacci_next()
{
    // check to see if we'd overflow result or position
    if ((ULLONG_MAX - previous_ < current_) ||
        (UINT_MAX == index_))
    {
        return false;
    }

    // Special case when index == 0, just return b value
    if (index_ > 0)
    {
        // otherwise, calculate next sequence value
        previous_ += current_;
    }
    std::swap(current_, previous_);
    ++index_;
    return true;
}

// Get the current value in the sequence.
unsigned long long fibonacci_current()
{
    return current_;
}

// Get the current index position in the sequence.
unsigned fibonacci_index()
{
    return index_;
}

bool iTest1() {
	
	std::cout << "ITEst1()\n";	
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