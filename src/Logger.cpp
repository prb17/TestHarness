#include "../include/Logger.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

//use answer here: https://stackoverflow.com/questions/49500419/how-to-use-a-mutex-as-a-member-variable-without-copy-constructor-of-simple-enc
//to implement copy constructor and copy assignment operator to properly copy the test harness' logger between threads

//possibly make an member variable for FILE* that handles the output
//Make it so the logger can be used by any other program, not just the harness
//add a config function that can adjust how the logger writes to the file/stdout
//add a check config properties
Logger::Logger()
{
	mFile = stdout;
	mLevel = Logger::LOG_LEVELS::LOW;
	prefix = "";
	return;
}

Logger::Logger(Logger const &other) {
	mFile = other.mFile;
	mLevel = other.mLevel;
	prefix = other.prefix;
}

Logger& Logger::operator=(Logger const& other)
{
	if (&other != this) {

		mFile = other.mFile;
		mLevel = other.mLevel;
		prefix = other.prefix;
	}
	return *this;
}

Logger::Logger(std::string file_name)
{
	fopen_s(&mFile, file_name.c_str(), "a");
	mLevel = Logger::LOG_LEVELS::LOW;
	prefix = "";
	return;
}

Logger::Logger(std::string file_name, Logger::LOG_LEVELS level)
{
	fopen_s(&mFile, file_name.c_str(), "a");
	mLevel = level;
	prefix = "";
	return;
}

Logger::Logger(std::string file_name, Logger::LOG_LEVELS level, std::string pref)
{
	fopen_s(&mFile, file_name.c_str(), "a");
	mLevel = level;
	prefix = pref;
	return;
}

void Logger::log(LOG_LEVELS level, std::string msg) {
	
	std::lock_guard<std::mutex> guard(loggerMutex);
	if (level <= mLevel)
	{
		if (mFile)
		{			
			fwrite(prefix.data(), sizeof(char), prefix.length(), mFile);
			fwrite(msg.data(), sizeof(char), msg.length(), mFile);
		}
	}
	return;
}
Logger::LOG_LEVELS Logger::get_level()
{
	return mLevel;
}

void Logger::set_level(Logger::LOG_LEVELS level) 
{
	mLevel = level;
}

void Logger::set_prefix(std::string pref) {
	prefix = pref;
}