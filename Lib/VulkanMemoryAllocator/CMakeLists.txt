cmake_minimum_required(VERSION 3.17)
project(VulkanMemoryAllocator)

include_directories(include)

add_library(VulkanMemoryAllocator vk_mem_alloc.cpp)