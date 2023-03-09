cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/Engine/Source/*.cpp"
        "${CMAKE_SOURCE_DIR}/Engine/Source/*.hpp"
        "${CMAKE_SOURCE_DIR}/Engine/Assets/Shaders/*.vert"
        "${CMAKE_SOURCE_DIR}/Engine/Assets/Shaders/*.frag"
        )

add_library(Engine STATIC ${SOURCE_FILES})

# Create source groups for each directory
foreach(FILE ${SOURCE_FILES})
    # Get the path relative to the source directory
    file(RELATIVE_PATH RELATIVE_FILE ${CMAKE_SOURCE_DIR} ${FILE})
    # Get the directory of the file
    get_filename_component(DIR ${RELATIVE_FILE} DIRECTORY)
    # Create the source group
    source_group(${DIR} FILES ${FILE})
endforeach()

set(ENGINE_ASSETS_DIR ${CMAKE_SOURCE_DIR}/Engine/Assets)
add_compile_definitions("ENGINE_ASSETS_DIR=\"${ENGINE_ASSETS_DIR}\"")
