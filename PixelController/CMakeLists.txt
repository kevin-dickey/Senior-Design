cmake_minimum_required(VERSION 3.26)
project(PixelController)

set(CMAKE_CXX_STANDARD 17)

add_executable(PixelController main.cpp)

add_subdirectory("${PROJECT_SOURCE_DIR}/ImGUI_Emulator")

# FastLED is not a CMake project, so we need to manually add the source files
set(FASTLED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/FastLED)
# TODO: Errors out - do we need to include this to build/use FastLED in the top-level project?
#add_subdirectory(${FASTLED_DIR})