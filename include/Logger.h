#pragma once
#include <string>

class Logger {
public:
	enum LOG_LEVELS {
		LOW,
		MED,
		HIGH
	};
	Logger(std::string file_name);
	Logger(std::string file_name, Logger::LOG_LEVELS level);
	void log(LOG_LEVELS level, std::string);
	void set_level(LOG_LEVELS level);
	LOG_LEVELS get_level(void);
private: 
	FILE* mFile;
	bool send_to_STDOUT
	LOG_LEVELS mLevel;
};
