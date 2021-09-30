cmake_minimum_required (VERSION 3.9)

set(CMAKE_BUILD_TYPE Release)

project(battleship)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -g")

set(SOURCES src/geometry.cpp src/geometry.hpp
	src/main.cpp
	src/plan.cpp src/plan.hpp)

add_executable(battleship ${SOURCES} src/mcts.h)
