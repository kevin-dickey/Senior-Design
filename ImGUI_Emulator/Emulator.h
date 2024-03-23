//
// Created by Nick Vazquez on 3/12/24.
//

#ifndef IMGUI_EMULATOR_EMULATOR_H
#define IMGUI_EMULATOR_EMULATOR_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Emulator {
public:
    void Init(GLFWwindow* window, const char* glsl_version);
    virtual void Update();
    void Render();
    void Shutdown();
};


#endif //IMGUI_EMULATOR_EMULATOR_H
