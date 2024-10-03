cmake_minimum_required(VERSION 3.26)
project(PixelController)
set(CMAKE_CXX_STANDARD 17)

# Add a preprocessor build directive which we use in the main source
# to change control flow based on platform.
add_compile_definitions(USE_EMULATOR)

# Included from: https://github.com/ocornut/imgui/wiki/Getting-Started
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# TODO: Make this into a library and link library to executable.
#   Then, the library can be linked to the embedded build through platform.
add_executable(
        PixelController src/main.cpp
        lib/configuration/Spatials.h
        lib/configuration/Layout.h
        lib/configuration/Sensor.h
        lib/configuration/Configuration.h
)

add_subdirectory("${PROJECT_SOURCE_DIR}/ImGUI_Emulator")

target_link_libraries(
        PixelController PRIVATE
        glad::glad
        glfw
        imgui::imgui
        ImGUI_Emulator_Lib
)
