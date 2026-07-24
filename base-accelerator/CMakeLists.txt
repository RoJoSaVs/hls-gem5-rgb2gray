cmake_minimum_required(VERSION 3.15)
project(rgb2gray CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED SYSTEMC_HOME)
    set(SYSTEMC_HOME "$ENV{SYSTEMC_HOME}")
endif()
if(NOT SYSTEMC_HOME)
    set(SYSTEMC_HOME "/usr/local/systemc")
endif()

message(STATUS "SYSTEMC_HOME = ${SYSTEMC_HOME}")

find_library(SYSTEMC_LIB
    NAMES systemc
    PATHS
        "${SYSTEMC_HOME}/lib"
        "${SYSTEMC_HOME}/lib64"
        "${SYSTEMC_HOME}/lib-linux64"
        "${SYSTEMC_HOME}/lib-linux"
        "/usr/lib64"
    REQUIRED
)
message(STATUS "SystemC lib  = ${SYSTEMC_LIB}")

find_path(SYSTEMC_INCLUDE_DIR
    NAMES systemc systemc.h
    PATHS
        "${SYSTEMC_HOME}/include"
        "${SYSTEMC_HOME}/include/systemc"
        "/usr/include"
        "/usr/include/systemc"
    REQUIRED
)
message(STATUS "SystemC include = ${SYSTEMC_INCLUDE_DIR}")

add_executable(rgb2gray src/main.cpp)

target_include_directories(rgb2gray PRIVATE
    "${SYSTEMC_INCLUDE_DIR}"
    "${SYSTEMC_INCLUDE_DIR}/systemc"
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(rgb2gray "${SYSTEMC_LIB}")

set_target_properties(rgb2gray PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
