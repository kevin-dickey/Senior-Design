//
// Created by Nick Vazquez on 3/23/24.
//

#include "PixelField.h"

PixelField::PixelField(PixelGridConfig config) {
    this->grid_config = config;
    this->window_padding = 5.0f;
    // Circle radius is calculated in the ShowPixelField method
    this->circle_radius = 0.0f;
    this->pixel_fills = new ImColor[config.num_cols * config.num_rows];
}

void PixelField::ShowPixelField() {
    ImGui::Begin("Canvas");                          // Create a window called "Conan Logo" and append into it.

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 window_size = ImGui::GetWindowSize();

    // Get the size of the main window
    ImGuiViewport* mainWindow = ImGui::GetMainViewport();

    // Calculate the minimum dimension
    float min_dimension = std::min(window_size.x, window_size.y);

    // Set the window size to keep the available area square
    ImGui::SetWindowSize(ImVec2(min_dimension, min_dimension));

    if (ImGui::Button("Reset View")) {
        ImGui::SetWindowSize(ImVec2(mainWindow->Size.x * 0.5f, mainWindow->Size.y * 0.5f));
    }

    float title_bar_height = ImGui::GetWindowSize().y - ImGui::GetContentRegionAvail().y;
    int padding = 20.0f;

    // Calculate the circle radius based on the smallest window dimension
    circle_radius = std::min(window_size.x, window_size.y - title_bar_height) / (2.0f * std::max(grid_config.num_rows,
                                                                                                 grid_config.num_cols));

    if (grid_config.grid_fill_state == CHANGING) {
        for (int i = 0; i < grid_config.num_cols * grid_config.num_rows; ++i) {
            // Fill the given pixel with a random color
            pixel_fills[i] = ImColor((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f);
        }
    }

    int total_circles = grid_config.num_cols * grid_config.num_rows;
    for (int col = 0; col < grid_config.num_cols; ++col) {
        for (int row = 0; row < grid_config.num_rows; ++row) {
            ImVec2 center(col * 2.0f * circle_radius + circle_radius + window_padding,
                          row * 2.0f * circle_radius + circle_radius + window_padding + title_bar_height);
            ImVec2 center_world = ImVec2(center.x + window_pos.x,
                                         center.y + window_pos.y);

            ImColor color;
            float hue = (col + row) / static_cast<float>(grid_config.num_cols + grid_config.num_rows - 2);
            float saturation = 1.0f;
            float value = 1.0f;

            switch (grid_config.grid_fill_state) {
                case RAINBOW:
                    float r, g, b;
                    ImGui::ColorConvertHSVtoRGB(hue, saturation, value, r, g, b);
                    color = ImColor(r, g, b);
                    break;
                case SOLID:
                    color = *grid_config.fill_color;
                    break;
                case CHANGING:
                    color = pixel_fills[row * grid_config.num_cols + col];
                    break;
            }

            draw_list->AddCircleFilled(center_world, circle_radius, color);

            std::string address = std::to_string(row * grid_config.num_cols + col + 1);
            ImVec2 text_size = ImGui::CalcTextSize(address.c_str());
            ImVec2 text_pos = ImVec2(center_world.x - text_size.x * 0.5f, center_world.y - text_size.y * 0.5f);
            draw_list->AddText(text_pos, IM_COL32(0, 0, 0, 255), address.c_str());
        }
    }
    ImGui::End();
}

