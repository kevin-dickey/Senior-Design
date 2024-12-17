#include "backgrounds.h"

void fillFrameWithColorPattern(
    CRGB* frame,          // 2D array of CRGB pixels 
    int height,            // Number of rows
    int width,             // Number of columns
    const std::vector<CRGB>& colors,  // Color pattern
    int stride,            // Width of each color block
    ShiftDirection shiftDirection 
) {
    // Ensure colors are not empty
    if (colors.empty()) return;

    // Calculate how many color blocks fit in the frame width
    int maxColorBlocks = width / stride;
    
    for (int row = 0; row < height; ++row) {
        // Determine the initial offset based on row and direction
        int offset = row * (shiftDirection == LEFT ? -1 : 1);
        
        // Normalize offset to be within a single pattern repeat
        int patternWidth = colors.size() * stride;
        offset = offset % patternWidth;
        if (offset < 0) offset += patternWidth;

        // Fill the current row
        for (int col = 0; col < width; ++col) {
            // Calculate which color block this column belongs to
            int relativePos = (col + offset) % patternWidth;
            int colorIndex = relativePos / stride;

            int originalIndex = (row * width) + col;
            
            // Ensure we don't go out of bounds of the colors vector
            if (colorIndex < colors.size()) {
                frame[originalIndex] = colors[colorIndex];
            }
        }
    }
}

// Version 2
// void fillFrameWithColorPattern(
//     CRGB** frame,          // 2D array of CRGB pixels 
//     int height,            // Number of rows
//     int width,             // Number of columns
//     const std::vector<CRGB>& colors,  // Color pattern
//     int stride,            // Width of each color block
//     Direction shiftDirection,
//     int skipFirstWidth = 0  // Offset for the first row's start
// ) {
//     // Validate inputs
//     if (colors.empty()) return;
//     if (skipFirstWidth >= stride) {
//         // Invalid skip width - should be less than stride
//         return;
//     }

//     // Calculate pattern width
//     int patternWidth = colors.size() * stride;
    
//     for (int row = 0; row < height; ++row) {
//         // Base offset calculation
//         int baseOffset = (row == 0 ? -skipFirstWidth : 0);
        
//         // Apply directional shift of one pixel per row
//         if (row > 0) {
//             // Shift based on direction
//             baseOffset += row * (shiftDirection == LEFT ? -1 : 1);
//         }
        
//         // Normalize offset to be within pattern repeat
//         int offset = baseOffset % patternWidth;
//         if (offset < 0) offset += patternWidth;

//         // Fill the current row
//         for (int col = 0; col < width; ++col) {
//             // Calculate which color block this column belongs to
//             int relativePos = (col + offset) % patternWidth;
//             int colorIndex = relativePos / stride;
            
//             // Ensure we don't go out of bounds of the colors vector
//             if (colorIndex < colors.size()) {
//                 frame[row][col] = colors[colorIndex];
//             }
//         }
//     }
// }