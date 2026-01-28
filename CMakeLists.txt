cmake_minimum_required(VERSION 3.16)
project(CrossEntropy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Charts)

add_executable(CrossEntropy main.cpp)
target_link_libraries(CrossEntropy PRIVATE Qt5::Widgets Qt5::Charts)
add_executable(main main.cpp)