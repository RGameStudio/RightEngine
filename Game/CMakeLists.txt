cmake_minimum_required(VERSION 3.19)

include_directories("../Engine/Source/Public")
include_directories("../Engine/Source/Core/Public")
include_directories("../Engine/Source/Launch/Public")

add_executable(Game Source/Start.cpp)
target_link_libraries(Game Engine)