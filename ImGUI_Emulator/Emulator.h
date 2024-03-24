//
// Created by Nick Vazquez on 3/12/24.
//

#ifndef IMGUI_EMULATOR_EMULATOR_H
#define IMGUI_EMULATOR_EMULATOR_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "PixelGridConfig.h"

class Emulator {
public:
    void Init(GLFWwindow* window, const char* glsl_version);
    void NewFrame();
    virtual void Update();
    void Render();
    void Shutdown();

private:



    PixelGridConfig grid_config;
    GRID_FILL_STATE grid_fill_state;
    float fill_color[3] = {1.0f, 0.0f, 0.2f };
    int num_cols;
    int num_rows;

};

#endif //IMGUI_EMULATOR_EMULATOR_H
