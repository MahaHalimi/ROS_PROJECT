cmake_minimum_required(VERSION 3.10)
project(ROS)


find_package(OpenCV 2.4 REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent Test OpenGL)

set(CMAKE_CXX_STANDARD 14)

add_executable(ROS imgUtils.cpp imgUtils.h line.cpp math.cpp math.h)
target_link_libraries(ROS ${OpenCV_LIBS} )
include_directories(${OpenCV_INCLUDE_DIRS})

