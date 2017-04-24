cmake_minimum_required(VERSION 2.8)

project(colorLines)

find_package(OpenCV REQUIRED)
include_directories(include)

file(GLOB SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS "-std=c++0x")
add_executable(main ${SOURCES})
target_link_libraries(main ${OpenCV_LIBS})
