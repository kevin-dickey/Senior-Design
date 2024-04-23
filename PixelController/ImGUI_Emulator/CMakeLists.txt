cmake_minimum_required(VERSION 3.5)
project(ImGUI_Emulator
    LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Included from: https://github.com/ocornut/imgui/wiki/Getting-Started
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(ImGUI_Emulator main.cpp
        Emulator.cpp
        Emulator.h
        PixelField.cpp
        PixelGridConfig.cpp
)

target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        glad::glad
        glfw
        imgui::imgui
)