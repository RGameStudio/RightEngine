cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)

include(GoogleTest)
enable_testing()
add_compile_definitions(R_TEST_BUILD)

file(GLOB_RECURSE SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/Tests/Source/*.cpp"
        "${CMAKE_SOURCE_DIR}/Tests/Source/*.hpp"
        )

add_executable(Tests ${SOURCE_FILES})
target_link_libraries(Tests Engine ${THIRD_PARTY_LIB} gtest)
gtest_discover_tests(Tests)

# Create source groups for each directory
foreach(FILE ${SOURCE_FILES})
    # Get the path relative to the source directory
    file(RELATIVE_PATH RELATIVE_FILE ${CMAKE_SOURCE_DIR} ${FILE})
    # Get the directory of the file
    get_filename_component(DIR ${RELATIVE_FILE} DIRECTORY)
    # Create the source group
    source_group(${DIR} FILES ${FILE})
endforeach()

set(TEST_ASSETS_DIR ${CMAKE_SOURCE_DIR}/Game/Assets)
add_compile_definitions("TEST_ASSETS_DIR=\"${TEST_ASSETS_DIR}\"")
