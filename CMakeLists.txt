cmake_minimum_required(VERSION 3.0.0)
project(inOneWeek VERSION 0.1.0 LANGUAGES C CXX)

# Specify the directories where the include files are
include_directories(include)
include_directories(external)


# Add all the source files in the src directory
file(GLOB SOURCES "src/*.cpp")

add_executable(inOneWeek ${SOURCES})