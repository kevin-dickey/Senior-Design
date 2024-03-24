//
// Created by Nick Vazquez on 3/24/24.
//

#ifndef IMGUI_EMULATOR_PIXELGRIDCONFIG_H
#define IMGUI_EMULATOR_PIXELGRIDCONFIG_H

#include <imgui.h>

enum GRID_FILL_STATE {
    RAINBOW,
    SOLID,
};

class PixelGridConfig {
public:
    GRID_FILL_STATE grid_fill_state = RAINBOW;
    // Changing this color doesn't affect anything because of the ColorEdit in Emulator.cpp overwriting the given val.
    ImColor *fill_color = new ImColor(1.0f, 0.0f, 0.2f);
    int num_cols = 0;
    int num_rows = 0;

    void configureGrid(int rows, int cols, GRID_FILL_STATE fill_state, ImColor *color);
    void configureGridSize(int num_rows, int num_cols);
};
#endif //IMGUI_EMULATOR_PIXELGRIDCONFIG_H
