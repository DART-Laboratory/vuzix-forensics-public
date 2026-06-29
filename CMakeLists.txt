cmake_minimum_required(VERSION 4.1)
project(tp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CLI11 CONFIG REQUIRED)

add_executable(tp
	main.cpp
	App.cpp
	Reader.cpp
	TimelineGenerator.cpp
	Builder.cpp
)

target_link_libraries(tp PRIVATE CLI11::CLI11)

