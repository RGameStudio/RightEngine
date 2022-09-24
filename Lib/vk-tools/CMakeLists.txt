cmake_minimum_required(VERSION 3.17)
project(vk-tools)

set(CMAKE_CXX_STANDARD 20)

set(VK_TOOLS_SRC
        vk-tools/VulkanTools.cpp
        )

add_library(vk-tools ${VK_TOOLS_SRC})