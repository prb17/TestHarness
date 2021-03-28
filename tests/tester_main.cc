#include<iostream>

#include <stdexcept>
#include <functional>
#include <cassert>
#include <thread>
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include "SimpleLogger.h"
#include "tests.h"
#include "zmq.h"
#include "czmq.h"

int main() {
	zsock_t *sock = zsock_new_pub(">tcp://127.0.0.1:9281");	
	std::this_thread::sleep_for(std::chrono::seconds(1));
	testObj to;
	to.x = 22;
	to.y = 33;
	
	std::string testObjStringified((char *)&to, 8);

	zmsg_t *msg = zmsg_new();
	zmsg_pushstr(msg, testObjStringified.c_str());
	zmsg_pushstr(msg, "test");

	zmsg_send(&msg, sock);
	std::this_thread::sleep_for(std::chrono::seconds(5));
	zmsg_destroy(&msg);
	zsock_destroy(&sock);
}