#include "Window.h"

const char *glsl_version;

GLFWwindow *createWindow() {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        throw std::runtime_error("Unable to initialize GLFW");

# if __APPLE__
    // GL 3.2 + GLSL 150
    glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
# else
    // GL 3.0 + GLSL 130
    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
# endif

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1920, 1280, "Dear ImGui - Emulator", NULL, NULL);
    if (window == nullptr)
        throw std::runtime_error("Unable to create GLFW window");

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))  // tie window context to glad's opengl funcs
        throw std::runtime_error("Unable to context to OpenGL");

    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    glViewport(0, 0, screen_width, screen_height);
    return window;
}

void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int emulator(MainloopCallback callback) {
    // Setup window
    GLFWwindow *window = createWindow();

    Emulator emulator;
    emulator.Init(window, glsl_version);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        callback();

        emulator.NewFrame();
        emulator.Update();
        emulator.Render();
        // Output the updated GLFW framebuffer to the window.
        glfwSwapBuffers(window);
    }
    emulator.Shutdown();
    return 0;
}

