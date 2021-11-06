cmake_minimum_required(VERSION 3.19)

include(GoogleTest)
enable_testing()
add_compile_definitions(R_TEST_BUILD)

add_executable(Tests Source/Runner/TestsRunner.cpp Source/Tests/CoreTests.cpp)
target_link_libraries(Tests EngineTest glad opengl32 easyargs gtest)

gtest_discover_tests(Tests)

add_custom_target(CopyShaderFilesToTests ALL DEPENDS ${GLSL_ROOT_DIR})

add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/Engine/Source/Shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Engine/Source/Shaders
        ${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders
        DEPENDS ${GLSL_SRC})

add_dependencies(Tests CopyShaderFilesToTests)