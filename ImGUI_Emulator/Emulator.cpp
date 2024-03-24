//
// Created by Nick Vazquez on 3/12/24.
//

#include <string>
#include "Emulator.h"

void Emulator::Init(GLFWwindow* window, const char* glsl_version) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

//    window_padding = 20.0f;
    window_padding = 0.0f;
    circle_margin = 10.0f;

    // Emulator state
    num_cols = 20;
    num_rows = 10;
}

void Emulator::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Emulator::Update() {
    ImGui::Begin("Canvas");                          // Create a window called "Conan Logo" and append into it.

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();

    float title_bar_height = ImGui::GetWindowSize().y - ImGui::GetContentRegionAvail().y;

    int padding = 20.0f;

    // Draw a grid of circles
    float circle_radius = 30.0f;
    int total_circles = num_cols * num_rows;
    for (int col = 0; col < num_cols; ++col) {
        for (int row = 0; row < num_rows; ++row) {
            ImVec2 center(col * 2.0f * circle_radius + circle_radius,
                          row * 2.0f * circle_radius + circle_radius + title_bar_height);
            ImVec2 center_world = ImVec2(center.x + window_pos.x + window_padding,
                                         center.y + window_pos.y + window_padding);

            float hue = (col + row) / static_cast<float>(num_cols + num_rows - 2);
            float saturation = 1.0f;
            float value = 1.0f;
            float r, g, b;
            ImGui::ColorConvertHSVtoRGB(hue, saturation, value, r, g, b);
            ImU32 color = ImColor(r, g, b);

            draw_list->AddCircleFilled(center_world, circle_radius, color);

            std::string address = std::to_string(row * num_cols + col + 1);
            ImVec2 text_size = ImGui::CalcTextSize(address.c_str());
            ImVec2 text_pos = ImVec2(center_world.x - text_size.x * 0.5f, center_world.y - text_size.y * 0.5f);
            draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), address.c_str());
        }
    }
    ImGui::End();
}

void Emulator::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Emulator::Shutdown() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
