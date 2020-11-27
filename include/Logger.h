#pragma once
#include <string>
#include <mutex>

class Logger {
public:
	enum LOG_LEVELS {
		LOW,
		MED,
		HIGH
	};
	Logger();
	Logger(Logger const& other);
	Logger& operator=(Logger const&);
	Logger(std::string file_name);
	Logger(std::string file_name, Logger::LOG_LEVELS level);
	Logger(std::string file_name, Logger::LOG_LEVELS level, std::string);
	void log(LOG_LEVELS level, std::string);
	void set_level(LOG_LEVELS level);
	LOG_LEVELS get_level(void);
	void set_prefix(std::string);
private: 
	FILE* mFile;
	LOG_LEVELS mLevel;
	std::string prefix;
	std::mutex loggerMutex;
};
