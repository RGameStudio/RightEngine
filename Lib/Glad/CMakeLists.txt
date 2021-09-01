cmake_minimum_required(VERSION 3.17)
project(glad C)

set(CMAKE_C_STANDARD 11)

set(GLAD_SOURCE
        include/glad/glad.h
        include/KHR/khrplatform.h
        src/glad.c
        )

include_directories(include)

add_library(glad ${GLAD_SOURCE})