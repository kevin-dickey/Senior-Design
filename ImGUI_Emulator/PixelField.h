//
// Created by Nick Vazquez on 3/23/24.
//

#ifndef IMGUI_EMULATOR_PIXELFIELD_H
#define IMGUI_EMULATOR_PIXELFIELD_H

#include <string>
#include <GLFW/glfw3.h>
#include "imgui.h"

#include "PixelGridConfig.h"

class PixelField {
public:
    PixelField(PixelGridConfig config);
    ImColor *pixel_fills;
    void ShowPixelField(); // Show the pixel field in the main window

private:
    PixelGridConfig grid_config;
    float window_padding;
    float circle_radius;
};


#endif //IMGUI_EMULATOR_PIXELFIELD_H
