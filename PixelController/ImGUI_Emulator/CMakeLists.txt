##############################
# ImGUI_Emulator CMake Project Definition
# Author: Nick Vazquez (nmv)
# Created: 4/20/2024 (hehe)
# ------------------------------
# Simple project for an ImGUI emulator for our senior design project.
# Provides both a library and executable target for the emulator.

cmake_minimum_required(VERSION 3.5)
project(ImGUI_Emulator
    LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Included from: https://github.com/ocornut/imgui/wiki/Getting-Started
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# NOTE: Multiple targets are declared here to provide a more modular build system.
#      This allows for easier testing and debugging of the emulator individually by building the source
#      files as either a library, or an executable and linking them to the same dependencies.
add_library(${PROJECT_NAME}_Lib main.cpp
        Emulator.cpp
        PixelField.cpp
        PixelGridConfig.cpp
)

add_executable(${PROJECT_NAME} main.cpp
        Emulator.cpp
        PixelField.cpp
        PixelGridConfig.cpp
)

list(APPEND Targets ${PROJECT_NAME} ${PROJECT_NAME}_Lib)
foreach(target IN LISTS Targets)
    target_link_libraries(
            ${target}
            PRIVATE
            glad::glad
            glfw
            imgui::imgui
    )
endforeach()
