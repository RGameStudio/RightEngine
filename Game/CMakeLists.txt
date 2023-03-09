cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/Game/Source/*.cpp"
        "${CMAKE_SOURCE_DIR}/Game/Source/*.hpp"
        )

add_executable(Game ${SOURCE_FILES})

if (MSVC)
    set(CMAKE_STARTUP_PROJECT Game)
endif()

# Create source groups for each directory
foreach(FILE ${SOURCE_FILES})
    # Get the path relative to the source directory
    file(RELATIVE_PATH RELATIVE_FILE ${CMAKE_SOURCE_DIR} ${FILE})
    # Get the directory of the file
    get_filename_component(DIR ${RELATIVE_FILE} DIRECTORY)
    # Create the source group
    source_group(${DIR} FILES ${FILE})
endforeach()

set_target_properties(Game
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(Game
        Engine
        ${THIRD_PARTY_LIB}
        )

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/Game/Assets)
add_compile_definitions("ASSETS_DIR=\"${ASSETS_DIR}\"")
