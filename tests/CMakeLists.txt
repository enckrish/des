find_package(Catch2 3 REQUIRED)
add_executable(tests_run ComponentTests.cpp
        DESTests.cpp
        utils.cpp
        benchmarks.cpp)

target_link_libraries(tests_run PRIVATE DES_lib)
target_link_libraries(tests_run PRIVATE Catch2::Catch2WithMain)
target_compile_definitions(tests_run PRIVATE UNIT_TEST)


include(Catch)
catch_discover_tests(tests_run)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")