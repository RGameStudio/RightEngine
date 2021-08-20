cmake_minimum_required(VERSION 3.19)

include_directories(Source/Core/Public)
include_directories(Source/Launch/Public)

add_library(Engine STATIC Source/Core/Public/Logger.hpp Source/Core/Logger.cpp Source/Launch/LaunchEngine.cpp Source/Launch/Public/LaunchEngine.hpp)