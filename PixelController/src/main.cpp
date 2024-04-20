#define USE_EMULATOR 1

#include <iostream>

# if USE_EMULATOR

#include <iostream>
#include "../ImGUI_Emulator/Emulator.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

# else

#include <FastLED.h>

#define LED_PIN         22
#define NUM_LEDS_X      16
#define NUM_LEDS_Y      16
#define NUM_LEDS        NUM_LEDS_X * NUM_LEDS_Y
#define MAX_BRIGHTNESS  16 // maximum for FastLED is 255, but I would probably not go higher than 64 (ESPECIALLY if no power supply)
#define COLOR_ORDER     GRB
#define CHIPSET         WS2812B
# endif



//void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y);
//
//uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
//
//uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter);
//
//uint16_t XY(uint8_t x, uint8_t y);
//
//uint16_t XYsafe(uint8_t x, uint8_t y);
//
///* Variables for XY() and XYsafe() */
//// Params for width and height
//const uint8_t kMatrixWidth = 16;
//const uint8_t kMatrixHeight = 16;
//// Param for different pixel layouts
//const bool kMatrixSerpentineLayout = true;
//const bool kMatrixVertical = false;

// Array of the LEDs. Should be accessed using the XY functions (translation to 2D array, which is not done directly b/c
//                                                               of different possible layouts of the LEDs (serpentine n such))
# if USE_EMULATOR
static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

# if __APPLE__
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
# else
    // GL 3.0 + GLSL 130
        const char *glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
# endif

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1920, 1280, "Dear ImGui - Emulator", NULL, NULL);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))  // tie window context to glad's opengl funcs
        throw ("Unable to context to OpenGL");

    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    glViewport(0, 0, screen_width, screen_height);

    Emulator emulator;
    emulator.Init(window, glsl_version);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        emulator.NewFrame();
        emulator.Update();
        emulator.Render();
    // Output the updated GLFW framebuffer to the window.
        glfwSwapBuffers(window);
    }
    emulator.Shutdown();
    return 0;
}
# else
CRGB leds[NUM_LEDS];

void setup() {
    Serial.begin(9600); // for setting up stuff to print to serial monitor
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(
            TypicalSMD5050); // setup the LEDs & LED pin for the esp32
    FastLED.setBrightness(MAX_BRIGHTNESS); // set the max brightness for the LEDs
}

void loop() {
    rippleEffect(255, 0, 255, NUM_LEDS_X / 2, NUM_LEDS_Y / 2); // purple :D
    FastLED.show();
    delay(75);  // adjust delay for speed of the ripple effect
}

/**
 * Provides a singular frame for the ripple effect.
 * Should be repeatedly called in a loop for a smooth ripple.
 * Currently has overlapping frames near the end of a ripple's lifespan (believe frames 11-13)
 *
 *    r = 0-255 value, specifying the amount of red
 *    g = 0-255 value, specifying the amount of green
 *    b = 0-255 value, specifying the amount of blue
 * 
 * Provide values 0-255 for specifying the color in terms of r, g, and b.
 * Do NOT adjust them for brightness, JUST COLOR. (nothing bad will happen just won't work as expected)
 * If you want to adjust the brightness of the LEDs, adjust MAX_BRIGHTNESS accordingly.
 * */
void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y) {
    static int rippleCounter = 0;
    // uint8_t center_x = NUM_LEDS_X / 2;
    // uint8_t center_y = NUM_LEDS_Y / 2;

    uint8_t maxDistance = max(NUM_LEDS_X / 2, NUM_LEDS_Y / 2);

    for (uint8_t x = 0; x < NUM_LEDS_X; x++) {
        for (uint8_t y = 0; y < NUM_LEDS_Y; y++) {
            uint8_t distance = calculateDistance(center_x, center_y, x, y);
            uint8_t rippleDistance = (rippleCounter + (maxDistance - distance)) % (maxDistance + 1);
            uint8_t brightness = 0;

            // Determine brightness based on distance from center and rippleCounter
            if (rippleDistance <= 1) {
                // If the pixel is within the ring
                brightness = MAX_BRIGHTNESS -
                             rippleDistance * 85; // Gradually decrease brightness towards the edge of the ring
            } else {
                // this part isn't really required (as brightness is set to 0 by default), but here for readability
                // If the pixel is outside the ring
                brightness = 0; // Dim brightness value (completely dark)
            }

            // set the desired color for the LED
            leds[XY(x, y)] = CRGB(r * brightness / MAX_BRIGHTNESS, g * brightness / MAX_BRIGHTNESS,
                                  b * brightness / MAX_BRIGHTNESS);  // Adjust color as needed
        }
    }

    // debugging
    Serial.printf("Now showing frame %d/13 of ripple %d.\n", (rippleCounter % 13 + 1), (rippleCounter / 13 + 1));
    rippleCounter++;
}

// void rippleEffect() {
//   static int rippleCounter = 0;
//   uint8_t center_x = NUM_LEDS_X / 2;
//   uint8_t center_y = NUM_LEDS_Y / 2;

//   for (int x = 0; x < NUM_LEDS; x++) {
//     for (int y = 0; y < NUM_LEDS; y++) {
//       uint8_t distance = calculateDistance(center_x, center_y, x, y);
//       uint8_t brightness = scaleBrightness(distance, rippleCounter);
//       leds[XYsafe(x, y)] = CRGB(0, 0, brightness); // just different brightnesses of blue
//     }
//   }

//   rippleCounter++;
// }

/**
 * Calculates the distance between two (x, y) points provided.
*/
uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    float dx = abs(x2 - x1);
    float dy = abs(y2 - y1);
    return sqrt(dx * dx + dy * dy);
}

/**
 * Scales the brightness of the LEDs based on the distance from the center of the actual ripple in the frame
 * For example, if the width of the ripple is 3 pixels wide, the center would be brightest and the 2 outside
 * pixels would be dimmed slightly. 
 * 
 * (Deprecated, should be moved to below soon unless use is found)
*/
uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter) {
    uint8_t delta = abs(rippleCounter - distance);
    uint8_t maxDistance = NUM_LEDS / 2;
    uint8_t brightness = map(delta, 0, maxDistance, 0, MAX_BRIGHTNESS);
    // return (brightness <= 0) ? 0 : (brightness > 2) ? 2 : brightness; // ensures 0 <= brightness <= 16
    return brightness > MAX_BRIGHTNESS ? 0 : brightness;
}

/**
 * Calculates the (x, y) position of a grid of LEDs.
 * This is required over just creating a 2D array as it depends whether the LEDs
 * are setup in a serpentine manner, and if they're setup ordered in a vertical manner or not.
 * 
 * If something doesn't look right, try changing the value of kMatrixVertical above.
 * If that doesn't work, try changing kMatrixSerpentineLayout (not applicable for testbench, we know the value it needs to be).
*/
uint16_t XY(uint8_t x, uint8_t y) {
    int i;

    if (kMatrixSerpentineLayout == false) {
        if (kMatrixVertical == false) {
            i = (y * kMatrixWidth) + x;
        } else {
            i = kMatrixHeight * (kMatrixWidth - (x + 1)) + y;
        }
    }

    if (kMatrixSerpentineLayout == true) {
        if (kMatrixVertical == false) {
            if (y & 0x01) {
                // Odd rows run backwards
                uint8_t reverseX = (kMatrixWidth - 1) - x;
                i = (y * kMatrixWidth) + reverseX;
            } else {
                // Even rows run forwards
                i = (y * kMatrixWidth) + x;
            }
        } else { // vertical positioning
            if (x & 0x01) {
                i = kMatrixHeight * (kMatrixWidth - (x + 1)) + y;
            } else {
                i = kMatrixHeight * (kMatrixWidth - x) - (y + 1);
            }
        }
    }

    return i;
}

/**
 * Makes sure the specified point is in bounds before calculating its (x, y) position.
*/
uint16_t XYsafe(uint8_t x, uint8_t y) {
    if (x >= kMatrixWidth) return -1;
    if (y >= kMatrixHeight) return -1;
    return XY(x, y);
}


/* *********************************************************************** */
// TODO:
/* Try these changes later */

// uint8_t calculateDistance(uint8_t center_x, uint8_t center_y, uint8_t x, uint8_t y) {
//   // Calculate the distances from the center to the edges of the matrix
//   uint8_t center_to_left_edge = center_x;
//   uint8_t center_to_right_edge = NUM_LEDS_X - 1 - center_x;
//   uint8_t center_to_top_edge = center_y;
//   uint8_t center_to_bottom_edge = NUM_LEDS_Y - 1 - center_y;

//   // Calculate the distances from the point (x, y) to the edges of the matrix
//   uint8_t point_to_left_edge = x;
//   uint8_t point_to_right_edge = NUM_LEDS_X - 1 - x;
//   uint8_t point_to_top_edge = y;
//   uint8_t point_to_bottom_edge = NUM_LEDS_Y - 1 - y;

//   // Calculate the minimum distances from the point to the center
//   uint8_t min_x_distance = min(abs(int(center_x) - int(x)), min(center_to_left_edge + point_to_right_edge, center_to_right_edge + point_to_left_edge));
//   uint8_t min_y_distance = min(abs(int(center_y) - int(y)), min(center_to_top_edge + point_to_bottom_edge, center_to_bottom_edge + point_to_top_edge));

//   return max(min_x_distance, min_y_distance);
// }

// void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y) {
//   static int rippleCounter = 0;
//   uint8_t maxDistance = max(NUM_LEDS_X, NUM_LEDS_Y);

//   for (uint8_t x = 0; x < NUM_LEDS_X; x++) {
//     for (uint8_t y = 0; y < NUM_LEDS_Y; y++) {
//       uint8_t distance = calculateDistance(center_x, center_y, x, y);
//       uint8_t rippleDistance = (rippleCounter + (maxDistance - distance)) % (maxDistance + 1);
//       uint8_t brightness;

//       // Determine brightness based on distance from center and rippleCounter
//       if (rippleDistance <= 1) {
//         // If the pixel is within the ring
//         brightness = MAX_BRIGHTNESS - rippleDistance * 85;  // Gradually decrease brightness towards the edge of the ring
//       } else {
//         // If the pixel is outside the ring
//         brightness = 0;  // Dim brightness value
//       }

//       leds[XY(x, y)] = CRGB(r * brightness / MAX_BRIGHTNESS, g * brightness / MAX_BRIGHTNESS, b * brightness / MAX_BRIGHTNESS);  // Adjust color as needed
//     }
//   }

//   // debugging
//   Serial.printf("Now showing frame %d/13 of ripple %d.\n", (rippleCounter % 13 + 1), (rippleCounter / 13 + 1));
//   rippleCounter++;
// }


// TODO:
/* Also try these changes later */

// uint8_t calculateDistance(uint8_t center_x, uint8_t center_y, uint8_t x, uint8_t y) {
//     int dx = abs(x - center_x);
//     int dy = abs(y - center_y);
//     return max(dx, dy); // Distance is the maximum of horizontal and vertical distances
// }

// void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y) {
//     static int rippleCounter = 0;

//     for (uint8_t x = 0; x < NUM_LEDS_X; x++) {
//         for (uint8_t y = 0; y < NUM_LEDS_Y; y++) {
//             uint8_t distance = calculateDistance(center_x, center_y, x, y);
//             uint8_t rippleDistance = (rippleCounter + distance) % 13;
//             uint8_t brightness;

//             // Determine brightness based on distance from center and rippleCounter
//             if (rippleDistance <= 1) {
//                 // If the pixel is within the ring
//                 brightness = MAX_BRIGHTNESS - rippleDistance * 85; // Gradually decrease brightness towards the edge of the ring
//             } else {
//                 // If the pixel is outside the ring
//                 brightness = 0; // Dim brightness value
//             }

//             leds[XY(x, y)] = CRGB(r * brightness / MAX_BRIGHTNESS, g * brightness / MAX_BRIGHTNESS, b * brightness / MAX_BRIGHTNESS);  // Adjust color as needed
//         }
//     }

//     // debugging
//     Serial.printf("Now showing frame %d/13 of ripple %d.\n", (rippleCounter % 13 + 1), (rippleCounter / 13 + 1));
//     rippleCounter++;
// }




/***********************************************************************************************************/
/* Deprecated code */

// #define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
// CRGB* const leds( leds_plus_safety_pixel + 1);

// Set 'kMatrixSerpentineLayout' to false if your pixels are 
// laid out all running the same way, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//     .----<----<----<----'
//     |
//     5 >  6 >  7 >  8 >  9
//                         |
//     .----<----<----<----'
//     |
//    10 > 11 > 12 > 13 > 14
//                         |
//     .----<----<----<----'
//     |
//    15 > 16 > 17 > 18 > 19
//
// Set 'kMatrixSerpentineLayout' to true if your pixels are 
// laid out back-and-forth, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//                         |
//     9 <  8 <  7 <  6 <  5
//     |
//     |
//    10 > 11 > 12 > 13 > 14
//                        |
//                        |
//    19 < 18 < 17 < 16 < 15
# endif
