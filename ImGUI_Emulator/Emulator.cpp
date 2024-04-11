//
// Created by Nick Vazquez on 3/12/24.
//

#include <string>
#include "PixelField.h"
#include "Emulator.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void Emulator::Init(GLFWwindow* window, const char* glsl_version) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Emulator state
    num_cols = 20;
    num_rows = 20;

    grid_fill_state = RAINBOW;
    grid_config.configureGrid(num_rows, num_cols, grid_fill_state, nullptr);
}

void Emulator::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Emulator::Update() {
    PixelField pixelField(grid_config);

    // Row Column Configuration Window
    ImGui::Begin("Configuration");
    ImGui::SliderInt("Rows", &num_rows, 1, 50);
    ImGui::SliderInt("Columns", &num_cols, 1, 50);

    ImGui::Text("Grid Fill State");
    ImGui::RadioButton("Rainbow", (int*)&grid_fill_state, RAINBOW);
    ImGui::RadioButton("Solid", (int*)&grid_fill_state, SOLID);
    ImGui::RadioButton("Random", (int*)&grid_fill_state, CHANGING);

    if (grid_fill_state == SOLID) {
        ImGui::ColorEdit3("Fill color", fill_color);
        ImGui::SameLine(); HelpMarker(
                "Click on the color square to open a color picker.\n"
                "Click and hold to use drag and drop.\n"
                "Right-click on the color square to show options.\n"
                "CTRL+click on individual component to input value.\n");
    }

    ImGui::End();

    ImVec4 fill_color_vec4 = ImVec4(fill_color[0], fill_color[1], fill_color[2], 1.0f);
    grid_config.configureGrid(num_rows, num_cols,
                              grid_fill_state, new ImColor(fill_color_vec4));
    pixelField.ShowPixelField();
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
