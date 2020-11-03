#pragma once
#include <string>

class Logger {
public:
	enum LOG_LEVELS {
		LOW,
		MED,
		HIGH
	};

	Logger();

	static Logger* Instance();

	void log(LOG_LEVELS, std::string, FILE*);

private: 
	static Logger *instance;
};
