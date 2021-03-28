add_executable(test_harness_tester tester_main.cc)
target_link_libraries(test_harness_tester LINK_PUBLIC 
    SimpleLogger
    zmq
    czmq
    pthread)