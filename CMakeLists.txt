cmake_minimum_required(VERSION 2.6)

project(libbfc)
project(bfc)

include_directories(./)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

add_library(libbfc SHARED libbfc.cpp)
add_executable(bfc bfc.cpp)

target_link_libraries(bfc libbfc)
