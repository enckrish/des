project(DES)

set(HEADER_FILES
        DES.h
)

set(SOURCE_FILES
        DES.cpp
        DES_tables.cpp
        DES.tpp
)

add_library(DES_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})
