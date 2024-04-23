#include "Window.h"


void test_loop_callback() {
    // Main loop
    std :: cout << "Hello, World!" << std :: endl;
}

int main() {
    emulator(test_loop_callback);
    return 0;
}
