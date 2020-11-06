#include "../include/Logger.h"
#include <stdio.h>

Logger* Logger::instance = nullptr;
//possibly make an member variable for FILE* that handles the output
//Make it so the logger can be used by any other program, not just the harness
//add a config function that can adjust how the logger writes to the file/stdout
//add a check config properties

Logger* Logger::Instance() {
	if (instance == nullptr) {
		instance = new Logger;
	}

	return instance;
}

Logger::Logger() {}

void Logger::log(LOG_LEVELS level, std::string msg, FILE *out_file) {
	fwrite(msg.data(), sizeof(char), msg.length(), out_file);
}