#include "Window.h"


int main() {
    GLFWwindow *window = Window::createWindow();

    Emulator emulator;
    emulator.Init(window, glsl_version);

    auto *clear_color = new ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        emulator.NewFrame();
        emulator.Update(clear_color);
        // NOTE: ImGui::Render() will render the frame, but it will not clear the framebuffer.
        // In the ImGui example, the framebuffer is cleared between the ImGui::Render() and [...]RenderDrawData() calls.
        Window::clearWindow(window, clear_color);
        emulator.Render();

        // Output the updated GLFW framebuffer to the window.
        glfwSwapBuffers(window);
    }
    emulator.Shutdown();
    return 0;
}
