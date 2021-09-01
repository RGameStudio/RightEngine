cmake_minimum_required(VERSION 3.19)
project(RightEngine)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(R_ENABLE_ASSERTS)

include_directories(Lib/spdlog/include)
include_directories(Engine/Source/Public)
include_directories(Engine/Source/Core/Public)
include_directories(Engine/Source/Core/Event/Public)
include_directories(Engine/Source/Core/Launch/Public)
include_directories(Engine/Source/Core/Window/Public)
include_directories(Engine/Source/Core/Application/Public)

add_subdirectory("Engine")
add_subdirectory("Game")
add_subdirectory("Lib/spdlog")