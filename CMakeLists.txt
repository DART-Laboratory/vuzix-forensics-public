cmake_minimum_required(VERSION 4.1)
project(hs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CLI11 CONFIG REQUIRED)

add_executable(hs
	main.cpp
	App.cpp
	Node.cpp
	LogcatLog.cpp
	SensorLog.cpp
	AudioServiceLog.cpp
	Reader.cpp
	TimelineGenerator.cpp
	Cleaner.cpp
	Builder.cpp
	Statistics.cpp
)

target_link_libraries(hs PRIVATE CLI11::CLI11)

