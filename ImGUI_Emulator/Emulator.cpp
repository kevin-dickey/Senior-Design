//
// Created by Nick Vazquez on 3/12/24.
//

#include "Emulator.h"

void Emulator::Init(GLFWwindow* window, const char* glsl_version) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Emulator::Update() {

}

void Emulator::Render() {

}

void Emulator::Shutdown() {

}
