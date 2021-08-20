cmake_minimum_required(VERSION 3.19)
project(RightEngine)

set(CMAKE_CXX_STANDARD 17)

include_directories(Lib/spdlog/include)

add_subdirectory("Engine")
add_subdirectory("Game")
add_subdirectory("Lib/spdlog")