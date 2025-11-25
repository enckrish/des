find_package(Catch2 3 REQUIRED)
add_executable(tests_run ComponentTests.cpp
        DESTests.cpp
        utils.cpp)

target_link_libraries(tests_run PRIVATE DES_lib)
target_link_libraries(tests_run PRIVATE Catch2::Catch2WithMain)
target_compile_definitions(tests_run PRIVATE UNIT_TEST)


include(Catch)
catch_discover_tests(tests_run)