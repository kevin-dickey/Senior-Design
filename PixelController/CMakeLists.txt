cmake_minimum_required(VERSION 3.26)
project(PixelController)

set(CMAKE_CXX_STANDARD 17)

add_executable(PixelController main.cpp)

# Not a CMake project, so we need to manually add the source files
# add_subdirectory("${PROJECT_SOURCE_DIR}/FastLED" "${PROJECT_SOURCE_DIR}/FastLED_output")
add_subdirectory("${PROJECT_SOURCE_DIR}/ImGUI_Emulator" "${PROJECT_SOURCE_DIR}/ImGUI_Emulator_output")