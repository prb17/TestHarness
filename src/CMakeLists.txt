
add_executable(testHarnessMain main.cc)
target_include_directories(testHarnessMain PUBLIC ${SimpleLogger_INCLUDE_DIRS} ${Utils_INCLUDE_DIRS})
target_link_libraries(testHarnessMain LINK_PUBLIC SimpleLogger pthread)
