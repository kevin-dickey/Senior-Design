cmake_minimum_required(VERSION 3.26)
project(PixelController)
set(CMAKE_CXX_STANDARD 17)

# Included from: https://github.com/ocornut/imgui/wiki/Getting-Started
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(PixelController main.cpp)

add_subdirectory("${PROJECT_SOURCE_DIR}/ImGUI_Emulator")

target_link_libraries(
        PixelController PRIVATE
        glad::glad
        glfw
        imgui::imgui
)

# FastLED is not a CMake project, so we need to manually add the source files
set(FASTLED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/FastLED)
# TODO: Errors out - do we need to include this to build/use FastLED in the top-level project?
# add_subdirectory(${FASTLED_DIR})