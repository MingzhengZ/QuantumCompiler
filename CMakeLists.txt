cmake_minimum_required(VERSION 3.14)
project(QuantumCompiler)

set(CMAKE_CXX_STANDARD 14)


add_executable(QuantumComper src/main.cpp src/Environment/Environment.cpp src/Environment/Environment.h
        src/Search/SearchNode.cpp src/Search/SearchNode.h src/Search/DefaultExpander.cpp src/Search/DefaultExpander.h
        src/Queue/Filter.h src/Queue/HashFilter_TOQM.hpp src/Queue/Queue.h src/Queue/DefaultQueue.hpp src/Queue/Queue1.hpp
        src/Search/Search.cpp src/Search/Search.h)
