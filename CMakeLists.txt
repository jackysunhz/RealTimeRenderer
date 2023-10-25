# CMakeList.txt : CMake project for RealTimeRenderer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(NAME RealTimeRenderer)

project(${NAME})

# Add source to this project's executable.
add_executable (${NAME} "src/main.cpp" )

# TODO: Add tests and install targets if needed.
