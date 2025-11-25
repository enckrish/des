project(DES)

set(HEADER_FILES
        DES.h
)

set(SOURCE_FILES
        DES.cpp
        tables.cpp
        utils.cpp
)

add_library(DES_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})