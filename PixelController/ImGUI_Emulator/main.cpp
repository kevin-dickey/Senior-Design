#include "Window.h"


int main() {
    GLFWwindow *window = Window::createWindow();

    Emulator emulator;
    emulator.Init(window, glsl_version);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        emulator.NewFrame();
        emulator.Update(new ImVec4(0.5, 0.5, 0.5, 1.0));
        // NOTE: ImGui::Render() will render the frame, but it will not clear the framebuffer.
        // In the ImGui example, the framebuffer is cleared between the ImGui::Render() and [...]RenderDrawData() calls.
        Window::clearWindow(window);
        emulator.Render();

        // Output the updated GLFW framebuffer to the window.
        glfwSwapBuffers(window);
    }
    emulator.Shutdown();
    return 0;
}
