cmake_minimum_required(VERSION 3.19)

add_library(Engine STATIC Source/Core/Public/Logger.hpp Source/Core/Logger.cpp Source/Launch/LaunchEngine.cpp Source/Launch/Public/LaunchEngine.hpp Source/Core/Event/Public/Event.hpp Source/Core/Event/Public/DemoEvent.hpp Source/Core/Event/EventDispatcher.cpp Source/Core/Event/Public/EventDispatcher.hpp Source/Core/Event/Public/ShutdownEvent.hpp)