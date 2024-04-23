//
// Created by Nick Vazquez on 4/20/24.
//

#ifndef PIXELCONTROLLER_WINDOW_H
#define PIXELCONTROLLER_WINDOW_H

#include <iostream>
#include "Emulator.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

extern const char *glsl_version;

typedef void (*MainloopCallback)();

static void glfw_error_callback(int error, const char *description);
GLFWwindow* createWindow();
int emulator(MainloopCallback callback);

#endif //PIXELCONTROLLER_WINDOW_H
