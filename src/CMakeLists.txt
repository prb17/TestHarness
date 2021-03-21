
target_include_directories(SimpleLogger PUBLIC ${SimpleLogger_INCLUDE_DIRS})

add_executable(testHarnessMain main.cc ThreadPool.cc)
target_link_libraries(testHarnessMain LINK_PUBLIC SimpleLogger)
