//
// Created by Nick Vazquez on 3/24/24.
//

#include "PixelGridConfig.h"

void PixelGridConfig::configureGrid(int rows, int cols, GRID_FILL_STATE fill_state, ImColor *color) {
    this->num_rows = rows;
    this->num_cols = cols;
    this->grid_fill_state = fill_state;
    this->fill_color = color;
}

void PixelGridConfig::configureGridSize(int rows, int cols) {
    this->num_rows = rows;
    this->num_cols = cols;
}
