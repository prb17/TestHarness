
add_executable(test_harness_main main.cc)
target_link_libraries(test_harness_main LINK_PUBLIC 
        SimpleLogger 
        zmq
        czmq
        pthread)
