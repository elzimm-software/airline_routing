cmake_minimum_required(VERSION 3.29)
project(airline_routing)

set(CMAKE_CXX_STANDARD 20)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/airports.csv ${CMAKE_CURRENT_BINARY_DIR}/airports.csv  COPYONLY)

add_executable(airline_routing main.cpp
        graph.h
        pathing.h
        tree.h
)
