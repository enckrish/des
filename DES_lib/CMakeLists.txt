project(DES)

set(HEADER_FILES
        DES.h
)

set(SOURCE_FILES
        DES.cpp
)

add_library(DES_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=[sanitizer_name] [additional_options] [-g] [-OX]")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")