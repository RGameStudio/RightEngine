cmake_minimum_required(VERSION 3.19)

add_executable(Game Source/Start.cpp)
target_link_libraries(Game Engine glad opengl32 gtest)