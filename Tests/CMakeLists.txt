cmake_minimum_required(VERSION 3.19)

add_compile_definitions(R_TEST_BUILD)

add_executable(Tests Source/Runner/TestsRunner.cpp Source/Tests/CoreTests.cpp)
target_link_libraries(Tests EngineTest glad opengl32 easyargs gtest)