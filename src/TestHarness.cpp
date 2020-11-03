#include "TestHarness.h"
#include <string>
#include <cassert>

TestHarness::TestHarness() {
	logger = logger->Instance();
	log_file_ptr = stdout;
}

TestHarness::TestHarness(std::string file_name) : log_file(file_name) {
	logger = logger->Instance();
	openLogFile();
}

bool TestHarness::openLogFile() {
	errno_t err;
	err = fopen_s(&log_file_ptr, log_file.c_str(), "a");
	if (err != 0) {
		logger->log(logger->HIGH, "Could not create handle for file i/o for file: '" + log_file + "' error code was: '" + std::to_string(err) + "'", stdout);
	}

	return err;
}