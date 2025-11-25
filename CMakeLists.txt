cmake_minimum_required(VERSION 4.0)
project(s_des)

set(CMAKE_CXX_STANDARD 20)

add_executable(s_des
        main.cpp
)

include_directories(DES_lib)
add_subdirectory(DES_lib)
target_link_libraries(s_des DES_lib)

add_subdirectory(tests)