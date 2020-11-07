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


	void log(LOG_LEVELS level, std::string, FILE*);
	void set_level(LOG_LEVELS level);
	LOG_LEVELS get_level(void);
private: 
	static Logger *instance;
	LOG_LEVELS mLevel;
};
