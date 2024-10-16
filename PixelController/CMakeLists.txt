cmake_minimum_required(VERSION 3.26)
project(PixelController)
set(CMAKE_CXX_STANDARD 17)

# Add a preprocessor build directive which we use in the main source
# to change control flow based on platform.
# Should be renamed to PLATFORM_CMAKE or something similar.
add_compile_definitions(USE_EMULATOR)
add_compile_definitions("SOURCE_ROOT=\"${CMAKE_SOURCE_DIR}\"")

# Add an option to enable or disable the ImGUI emulator.
option(USE_IMGUI "Include ImGUI Packages in the build" ON)

# Add the include directory to the project.
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib)

# Included from: https://github.com/ocornut/imgui/wiki/Getting-Started
if (USE_IMGUI)
    message(STATUS "Building with ImGUI")
    add_compile_definitions(USE_IMGUI)
    find_package(glad CONFIG REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(imgui CONFIG REQUIRED)
endif ()

add_library(ConfigurationLib
        lib/configuration/Spatials.h
        lib/configuration/Layout.h
        lib/configuration/Sensor.h
        lib/configuration/Configuration.h
        lib/configuration/Configuration.cpp
)

add_library(ImageProcessingLib
        lib/imageProcessing/ImageProcessing.h
        lib/imageProcessing/ImageProcessing.cpp
)

# TODO: Make this into a library and link library to executable.
#   Then, the library can be linked to the embedded build through platform.
add_executable(PixelController src/main.cpp)

if (USE_IMGUI)
    target_compile_definitions(PixelController PRIVATE USE_IMGUI)
    add_subdirectory("${PROJECT_SOURCE_DIR}/ImGUI_Emulator")
endif ()

target_link_libraries(
        PixelController PRIVATE
        ConfigurationLib
        ImageProcessingLib
)

if (USE_IMGUI)
    message(STATUS "Linking ImGUI Libraries")
    include_directories(${ImGUI_Emulator_SOURCE_DIR})
    target_link_libraries(
            PixelController PRIVATE
            glad::glad
            glfw
            imgui::imgui
            ImGUI_Emulator_Lib
    )
endif ()
