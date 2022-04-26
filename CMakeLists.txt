cmake_minimum_required(VERSION 3.21)
project(QuantumComper)

set(CMAKE_CXX_STANDARD 14)

add_executable(QuantumComper src/main.cpp src/Environment/Environment.cpp src/Environment/Environment.h src/Search/SearchNode.cpp src/Search/SearchNode.h src/Search/DefaultExpander.cpp src/Search/DefaultExpander.h src/Search/Expander.h src/Queue/Filter.h src/Queue/HashFilter_TOQM.hpp)
