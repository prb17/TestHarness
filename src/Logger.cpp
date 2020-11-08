#include "../include/Logger.h"
#include <stdio.h>

Logger* Logger::instance = nullptr;
//possibly make an member variable for FILE* that handles the output
//Make it so the logger can be used by any other program, not just the harness
//add a config function that can adjust how the logger writes to the file/stdout
//add a check config properties
Logger::Logger(std::string file_name)
{
	fopen_s(&mFile, file_name.c_str(), "a");
	mLevel = Logger::LOG_LEVELS::LOW;
	return;
}
Logger::Logger(std::string file_name, Logger::LOG_LEVELS level)
{
	fopen_s(&mFile, file_name.c_str(), "a");
	mLevel = level;
	return;
}
void Logger::log(LOG_LEVELS level, std::string msg) {
	if ((int)level > (int)mLevel)
	{
		if (mFile)
		{
			fwrite(msg.data(), sizeof(char), msg.length(), mFile);
		}
		else
		{
			fwrite("ERROR FILE HANDLE NOT SETUP CORRECTLY:" + msg.data(), sizeof(char), msg.length + 39, stdout);
		}
	}
	return;
}
Logger::LOG_LEVELS Logger::get_level()
{
	return mLevel;
}