//
// Created by Nick Vazquez on 4/20/24.
//

#ifndef PIXELCONTROLLER_WINDOW_H
#define PIXELCONTROLLER_WINDOW_H

#include <iostream>
#include "Emulator.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define GL_SILENCE_DEPRECATION

extern const char *glsl_version;

class Window {
public:
    static void glfw_error_callback(int error, const char *description);
    static GLFWwindow* createWindow();
    static void clearWindow(GLFWwindow *window, ImVec4 *clear_color);
};

#endif //PIXELCONTROLLER_WINDOW_H
