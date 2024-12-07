#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

#include <FileManager.h>

#define STRIP_1_PIN 32
#define STRIP_2_PIN 33
#define STRIP_3_PIN 25
#define STRIP_4_PIN 26
#define STRIP_5_PIN 27
#define STRIP_6_PIN 14
#define STRIP_7_PIN 12

#define FRAMES_PER_SECOND 30 
#define NUM_LEDS_X 100 
#define NUM_LEDS_Y 7 
#define NUM_LEDS NUM_LEDS_X *NUM_LEDS_Y
#define MAX_BRIGHTNESS 16 // maximum for FastLED is 255, (don't go higher than like 8 if you don't have a PSU attached)

#if USE_EMULATOR
#define PROJECT_DIR SOURCE_ROOT

// TODO: Gotta do this for anything in lib while using CMake. PlatformIO automatically adds the include path for lib
//  so that we can include the headers directly (<Configuration.h>). This is a workaround for CMake.
#include "configuration/Configuration.h"
#include "configuration/FileManager.h"
#include "imageProcessing/ImageProcessing.h"

#else
#define COLOR_ORDER GRB
#define CHIPSET WS2812B

#include <FastLED.h>

#include <Configuration.h>
#include <ImageProcessing.h>
#include "rippleEffect.h"
#include "utils.h"

#endif

FileManager *fileManager;
Show_t show;

#if USE_EMULATOR
void loadExampleImages(const std::vector<std::string *> &image_paths,
                       std::vector<ImageProcessing::ImageData_t> &out_loadedImages);

void resizeImages(const std::vector<ImageProcessing::ImageData_t> &loadedImages, GridLayout *gridLayout,
                  std::vector<ImageProcessing::ImageData_t> &resizedImages);

int main()
{
    std::string showFilePath = std::string(PROJECT_DIR) + "/lib/configuration/test/Basic_Show_File.json";
    std::string risingSunFilePath = std::string(PROJECT_DIR) + "/test/rising_sun.png";
    std::string djiboutiFilepath = std::string(PROJECT_DIR) + "/test/djibouti.jpg";
    // Array of image paths
    auto imagePaths = std::vector<std::string *>{&risingSunFilePath, &djiboutiFilepath};

    auto loadedImages = std::vector<ImageProcessing::ImageData_t>();
    auto resizedImages = std::vector<ImageProcessing::ImageData_t>();

    show = loadShow(showFilePath);
    loadExampleImages(imagePaths, loadedImages);
    resizeImages(loadedImages, dynamic_cast<GridLayout *>(show.layouts[0].get()), resizedImages);

    // Print info about the loaded images
    for (auto &i : resizedImages)
    {
        std::cout << "Resized image: " << i.width << "x" << i.height << "x" << i.channels << std::endl;
    }

    // Free the loaded images
    for (auto &i : loadedImages)
    {
        ImageProcessing::free_image(i.data);
    }

    // Free the resized images
    for (auto &i : resizedImages)
    {
        ImageProcessing::free_image(i.data);
    }

    return 0;
}

void loadExampleImages(const std::vector<std::string *> &image_paths,
                       std::vector<ImageProcessing::ImageData_t> &out_loadedImages)
{
    // Clear the loaded images
    out_loadedImages.clear();

    for (const auto &i : image_paths)
    {
        // Load the imagePath
        std::string imagePath = *i;

        int ok, width, height, channels;
        ok = ImageProcessing::get_image_dimensions(imagePath.c_str(), &width, &height, &channels);
        if (!ok)
        {
            std::cerr << "Error getting imagePath dimensions" << std::endl;
            return;
        }

        unsigned char *data = ImageProcessing::load_image(imagePath.c_str(), &width, &height, &channels);
        if (data == nullptr)
        {
            std::cerr << "Error loading imagePath: " << imagePath << std::endl;
            return;
        }

        std::cout << "Image Filename: " << imagePath << std::endl;
        std::cout << "Image Width: " << width << std::endl;
        std::cout << "Image Height: " << height << std::endl;
        std::cout << "Image Channels: " << channels << std::endl;

        // Store the loaded image
        out_loadedImages.push_back({data, width, height, channels});
    }

    std::cout << "Loaded " << out_loadedImages.size() << " images" << std::endl;
}

void resizeImages(const std::vector<ImageProcessing::ImageData_t> &loadedImages, GridLayout *gridLayout,
                  std::vector<ImageProcessing::ImageData_t> &resizedImages)
{
    // Resize the loaded images to the grid layout size
    for (auto &i : loadedImages)
    {
        int resize_width = gridLayout->width;
        int resize_height = gridLayout->height;

        unsigned char *resizedImage = ImageProcessing::resize_image(i.data, i.width, i.height, i.channels,
                                                                    resize_width, resize_height);
        if (resizedImage == nullptr)
        {
            std::cerr << "Error resizing image" << std::endl;
            return;
        }

        // Store the resized image
        resizedImages.push_back({resizedImage, resize_width, resize_height, i.channels});
    }
}

#else

/**
 * MARK: Setup
 */

uint8_t bufferToCRGBArray(unsigned char* buffer, int imgWidth, int imgHeight, int imgChannels, CRGB* leds, int matrixWidth, int matrixHeight, int startX, int startY, bool wrap = false);
void fillRemainingPixels(CRGB *leds, int matrixWidth, int matrixHeight, CRGB backgroundColor);

void loadHexBitmap(CRGB *leds, const char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth);
CRGB hexToCRGB(const char *hex);

void printCRGB(const CRGB &color) {
    std::cout << "(" << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ")";
}

// Array of the LEDs. Should be accessed using the XY functions (translation to 2D array, which is not done directly b/c
//                                                               of different possible layouts of the LEDs (serpentine n such))
CRGB frame_data[NUM_LEDS];
CRGB strip_data[NUM_LEDS_Y][NUM_LEDS_X];

unsigned char bufferPattern[3][3][3] = {
    {{0x00, 0x00, 0xFF}, {0x00, 0x00, 0xFF}, {0x00, 0xFF, 0x00}},
    {{0x00, 0x00, 0xFF}, {0x00, 0xFF, 0x00}, {0xFF, 0xFF, 0xFF}},
    {{0x00, 0xFF, 0x00}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}}};

// Pointer to the array
unsigned char *bufferPtr = &bufferPattern[0][0][0];

int loadedImageHeight, loadedImageWidth, loadedImageChannels;
unsigned char *loadedImage;

uint8_t hue = 0;
const uint8_t deltaHue = 8, hueDensity = 2;

void setup()
{
    Serial.begin(115200);                    // for setting up stuff to print to serial monitor
    // delay(3000);                             // delay for 3 seconds to give time to open the serial monitor
    std::cout << "Starting..." << std::endl; // print to the serial monitor that the program is starting

    // SD Card Setup
    FileManager *fm = new FileManager();
    if (!fm->MountFileSystem())
    {
        std::cout << "Failed to mount file system. Halting..." << std::endl;
        while (true)
            ;
    }

    // FastLED Initialization
    FastLED.addLeds<CHIPSET, STRIP_1_PIN, COLOR_ORDER>(strip_data[0], NUM_LEDS_X).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_2_PIN, COLOR_ORDER>(strip_data[1], NUM_LEDS_X).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_3_PIN, COLOR_ORDER>(strip_data[2], NUM_LEDS_X).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_4_PIN, COLOR_ORDER>(strip_data[3], NUM_LEDS_X).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_5_PIN, COLOR_ORDER>(strip_data[4], NUM_LEDS_X).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_6_PIN, COLOR_ORDER>(strip_data[5], NUM_LEDS_X).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_7_PIN, COLOR_ORDER>(strip_data[6], NUM_LEDS_X).setCorrection(TypicalSMD5050);
    // FastLED.addLeds<CHIPSET, STRIP_8_PIN, COLOR_ORDER>(strip_data[7], NUM_LEDS_X).setCorrection(TypicalSMD5050);

    FastLED.setBrightness(MAX_BRIGHTNESS);                                                        // set the max brightness for the LEDs
    pinMode(LED_BUILTIN, OUTPUT);                                                                 // setup the built-in LED for the esp32
    fill_solid(frame_data, NUM_LEDS, CRGB::Black);
    FastLED.show();
    Serial.println("Initialized FastLED...");

    std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;

    try
    {
        std::cout << "⏳ Loading Show..." << std::endl;
        File showFile = fm->getJsonFile("/christmas-y.json");
        show = loadShow(showFile);
        std::cout << "✅ Loaded Show!" << std::endl;
        // std::cout << "➡️ Show Name: " << show.name << std::endl;
        showFile.close();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading show: " << e.what() << std::endl;
    }

    // try
    // {
    //     long start_loading_ghost = millis();

    //     std::cout << "⏳ Loading Ghost..." << std::endl;
    //     File ghostBmpFile = fm->getJsonFile("/djibouti.jpg");
    //     std::cout << "✅ Opened Ghost File!" << std::endl;

    //     std::cout << "Converting to FILE..." << std::endl;
    //     size_t fileSize;
    //     unsigned char *ghostFileBuf = ImageProcessing::convertFsFileToBuffer(&ghostBmpFile, fileSize);
    //     std::cout << "✅ Converted to FILE!" << std::endl;
    //     std::cout << "  File Size: " << fileSize << std::endl;
    //     std::cout << "  First 10 bytes: " << ghostFileBuf[0] << ghostFileBuf[1] << ghostFileBuf[2] << ghostFileBuf[3] << ghostFileBuf[4] << ghostFileBuf[5] << ghostFileBuf[6] << ghostFileBuf[7] << ghostFileBuf[8] << ghostFileBuf[9] << std::endl;

    //     std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;
    //     std::cout << "🛑 Closing ghost from file manager..." << std::endl;
    //     ghostBmpFile.close();
    //     std::cout << "✅ Closed ghost from file manager!" << std::endl;

    //     std::cout << "⏳ Getting Image Dimensions..." << std::endl;

    //     // Get the dimensions of the image and load it
    //     ImageProcessing::get_image_dimensions_from_memory(ghostFileBuf, fileSize, &loadedImageWidth, &loadedImageHeight, &loadedImageChannels);

    //     std::cout << "  Image Width: " << loadedImageWidth << std::dec << std::endl;
    //     std::cout << "  Image Height: " << loadedImageHeight << std::dec << std::endl;
    //     std::cout << "  Image Channels: " << loadedImageChannels << std::dec << std::endl;

    //     unsigned char *ghostFile = ImageProcessing::load_image_from_memory(ghostFileBuf, fileSize, &loadedImageWidth, &loadedImageHeight, &loadedImageChannels);
    //     std::cout << "✅ Loaded Image!" << std::endl;
    //     std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;

    //     std::cout << "↔️ Resizing Image..." << std::endl;
    //     int newWidth = NUM_LEDS_X;
    //     int newHeight = NUM_LEDS_Y;
    //     unsigned char *resizedGhost = ImageProcessing::resize_image(ghostFile, loadedImageWidth, loadedImageHeight, loadedImageChannels, newWidth, newHeight, true);
    //     std::cout << "🕊️ Freeing Image & Buffer..." << std::endl;
    //     ImageProcessing::free_image(ghostFile);
    //     free(ghostFileBuf);

    //     // DEBUG: Display the loaded image data
    //     std::cout << "🖼️ Displaying Image Data..." << std::endl;
    //     std::cout << "  New Width: " << newWidth << std::endl;
    //     std::cout << "  New Height: " << newHeight << std::endl;
    //     ImageProcessing::printImageHex(resizedGhost, newWidth, newHeight, loadedImageChannels);

    //     // Log the time taken to load the ghost
    //     long end_loading_ghost = millis();
    //     std::cout << "⏲️ Time taken to load ghost: " << end_loading_ghost - start_loading_ghost << "ms" << std::endl;

    //     // Load the resized image into the LED matrix
    //     std::cout << "🚦 Loading Image into LED Array..." << std::endl;
    //     // CRGB *serpentineArray = (CRGB *)calloc(NUM_LEDS, sizeof(CRGB));
    //     // Load the test pattern
    //     // bufferToCRGBArray(bufferPtr, 3, 3, 3, serpentineArray, NUM_LEDS_X, NUM_LEDS_Y, 16, 16, true);
    //     // Load the resized ghost
    //     bufferToCRGBArray(resizedGhost, newWidth, newHeight, loadedImageChannels, frame_data, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
    //     fillRemainingPixels(frame_data, NUM_LEDS_X, NUM_LEDS_Y, CRGB::GreenYellow);
        
    //     // Convert the 2D array to a CRGB**
    //     CRGB* stripDataPtrs[NUM_LEDS_Y];
    //     for (int i = 0; i < NUM_LEDS_Y; ++i) {
    //         stripDataPtrs[i] = strip_data[i];
    //     }

    //     rearrangeForStrips(frame_data, stripDataPtrs, NUM_LEDS_X, NUM_LEDS_Y);
    //     FastLED.show();

    //     // TODO: `free` or free_image causes a kernel panic, possibly due to a double free, the large size, or something else.
    //     //  Not freeing will cause a memory leak, but it's better than a kernel panic for now.
    //     // std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;
    //     // std::cout << "🕊️ Freeing Loaded Image..." << std::endl;
    //     // free(resizedGhost);
    //     // std::cout << "✅ Freed Image!" << std::endl;
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Error loading ghost: " << e.what() << std::endl;
    // }
}

/**
 * MARK: Looping
 */
void loop()
{
    // gets the current state of every sensor, the state automatically resets after it's viewed
    auto current_millis = millis();

    // Use fill_rainbow to fill the LEDs with a rainbow pattern. Each strip should be offset from the previous one by 1/7th of the rainbow.
    // The rainbow should "chase" around the strips.
    for (int i = 0; i < NUM_LEDS_Y; ++i)
    {
        fill_rainbow(strip_data[i], NUM_LEDS_X, hue += hueDensity, deltaHue);
    }
    FastLED.show();
    delay(1000 / FRAMES_PER_SECOND);
}

// Function to load an 8x8 bitmap from a hex string
// startX and startY specify the position of the top-right of the bitmap to load in
void loadHexBitmap(CRGB *leds, const char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth)
{
    for (uint8_t y = 0; y < bitmapHeight; y++)
    {
        for (uint8_t x = 0; x < bitmapWidth; x++)
        {
            // Calculate the position in the string
            int index = (y * bitmapWidth + x) * 7; // 6 for color + 1 for space

            // Extract the hex color (6 characters)
            char hexColor[7]; // 6 for color + 1 for null terminator
            strncpy(hexColor, &bitmap[index], 6);
            hexColor[6] = '\0'; // Null-terminate the string

            // Only load if within bounds
            if (startX + x < kMatrixWidth && startY + y < kMatrixHeight)
            {
                CRGB color = hexToCRGB(hexColor);
                if (color != (CRGB::Black))
                { // black color is interpreted as intending to be transparent
                    leds[XY(startX + x, startY + y)] = color;
                }
            }
        }
    }
    FastLED.show();
}

// Function to convert a 6-character hex string to CRGB
CRGB hexToCRGB(const char *hex)
{
    uint8_t r = strtol(std::string(hex, 2).c_str(), NULL, 16);
    uint8_t g = strtol(std::string(hex + 2, 2).c_str(), NULL, 16);
    uint8_t b = strtol(std::string(hex + 4, 2).c_str(), NULL, 16);
    return CRGB(r, g, b);
}

uint8_t bufferToCRGBArray(unsigned char* buffer, int imgWidth, int imgHeight, int imgChannels, CRGB* leds, int matrixWidth, int matrixHeight, int startX, int startY, bool wrap) {
    // int startX = (matrixWidth - imgWidth) / 2;
    // int startY = (matrixHeight - imgHeight) / 2;
    if (startX < 0 || startY < 0) {
        return 1;
    }

    if (startX >= matrixWidth || startY >= matrixHeight) {
        return 1;
    }

    for (int y = 0; y < imgHeight; ++y) {
        for (int x = 0; x < imgWidth; ++x) {
            int bufferIndex = (y * imgWidth + x) * imgChannels;
            uint8_t matrixIndex = 0;
            if (wrap) {
               matrixIndex = ((startY + y) % matrixHeight) * matrixWidth + ((startX + x) % matrixWidth); 
            } else {
                matrixIndex = (startY + y) * matrixWidth + (startX + x);
            }

            if (imgChannels == 3) { // RGB
                leds[matrixIndex] = CRGB(buffer[bufferIndex], buffer[bufferIndex + 1], buffer[bufferIndex + 2]);
            } else if (imgChannels == 4) { // RGBA
                leds[matrixIndex] = CRGB(buffer[bufferIndex], buffer[bufferIndex + 1], buffer[bufferIndex + 2]);
            }
        }
    }
    return 0;
}


void fillRemainingPixels(CRGB *leds, int matrixWidth, int matrixHeight, CRGB backgroundColor)
{
    for (int y = 0; y < matrixHeight; ++y)
    {
        for (int x = 0; x < matrixWidth; ++x)
        {
            int index = y * matrixWidth + x;
            if (leds[index] == CRGB::Black)
            {
                leds[index] = backgroundColor;
            }
        }
    }
}
#endif
