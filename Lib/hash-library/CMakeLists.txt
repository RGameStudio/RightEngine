cmake_minimum_required(VERSION 3.17)
project(hash-library)

set(CMAKE_CXX_STANDARD 20)

set(HASH_LIBRARY_SRC
        sha256.cpp
        )

add_library(hash-library ${HASH_LIBRARY_SRC})