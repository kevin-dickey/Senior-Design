#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

#include <FileManager.h>

#define LED_PIN 13 
#define NUM_LEDS_X 16
#define NUM_LEDS_Y 16
#define NUM_LEDS NUM_LEDS_X * NUM_LEDS_Y 
#define MAX_BRIGHTNESS 8 // maximum for FastLED is 255, (don't go higher than like 8 if you don't have a PSU attached)

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

enum ShiftDirection
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

FileManager *fileManager;
Show_t show;

// Function prototypes
void loadExampleImages(const std::vector<std::string *> &image_paths,
                       std::vector<ImageProcessing::ImageData_t> &out_loadedImages);

void resizeImages(const std::vector<ImageProcessing::ImageData_t> &loadedImages, GridLayout *gridLayout,
                  std::vector<ImageProcessing::ImageData_t> &resizedImages);

#if USE_EMULATOR

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

#else

/**
 * MARK: Setup
 */

void fadeToBlack(int duration);
void fadeToBrightness(int duration, int targetBrightness);

void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8);              // draws rainbow frame
void DrawOneFrameReducedBright(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8); // ^ @ half brightness

void bufferToCRGBArray(unsigned char* buffer, int imgWidth, int imgHeight, int imgChannels, CRGB* leds, int matrixWidth, int matrixHeight);
void fillRemainingPixels(CRGB* leds, int matrixWidth, int matrixHeight, CRGB backgroundColor);

void parseBitmapData(const char *hexData);
void loadHexBitmap(CRGB *leds, const char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth);
void loadByteBitmap(CRGB *leds, const unsigned char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth);
CRGB hexToCRGB(const char *hex);

void shiftLeds(CRGB leds[], ShiftDirection direction);

// Array of the LEDs. Should be accessed using the XY functions (translation to 2D array, which is not done directly b/c
//                                                               of different possible layouts of the LEDs (serpentine n such))
CRGB leds[NUM_LEDS];
int prevLeds1[NUM_LEDS] = {0};
int prevLeds2[NUM_LEDS] = {0};
int prevLeds3[NUM_LEDS] = {0};
int prevLeds4[NUM_LEDS] = {0};

const char *skull8bit = "000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000 ffffff ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff ffffff 000000 000000 000000 000000 000000 000000 000000 ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff 000000 000000 000000 000000 000000 000000 000000 000000 ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff 000000 000000 000000 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000";

int loadedImageHeight, loadedImageWidth, loadedImageChannels;
unsigned char *loadedImage;

void setup()
{
    Serial.begin(115200);                    // for setting up stuff to print to serial monitor
    delay(3000);                             // delay for 3 seconds to give time to open the serial monitor
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
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050); // setup the LEDs & LED pin for the esp32
    FastLED.setBrightness(MAX_BRIGHTNESS);                                                        // set the max brightness for the LEDs
    pinMode(LED_BUILTIN, OUTPUT);                                                                 // setup the built-in LED for the esp32
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    Serial.println("Initialized FastLED...");

    std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;

    long start_loading_ghost = millis();

    std::cout << "⏳ Loading Ghost..." << std::endl;
    File ghostBmpFile = fm->getJsonFile("/djibouti.jpg");
    std::cout << "✅ Opened Ghost File!" << std::endl;
    std::cout << "Converting to FILE..." << std::endl;
    size_t fileSize;
    unsigned char *ghostFileBuf = ImageProcessing::convertFsFileToBuffer(&ghostBmpFile, fileSize);
    std::cout << "✅ Converted to FILE!" << std::endl;
    std::cout << "  File Size: " << fileSize << std::endl;
    std::cout << "  First 10 bytes: " << ghostFileBuf[0] << ghostFileBuf[1] << ghostFileBuf[2] << ghostFileBuf[3] << ghostFileBuf[4] << ghostFileBuf[5] << ghostFileBuf[6] << ghostFileBuf[7] << ghostFileBuf[8] << ghostFileBuf[9] << std::endl;

    std::cout << "⏳ Getting Image Dimensions..." << std::endl;
    // Get the dimensions of the image and load it
    ImageProcessing::get_image_dimensions_from_memory(ghostFileBuf, fileSize, &loadedImageWidth, &loadedImageHeight, &loadedImageChannels);

    std::cout << "  Image Width: " << loadedImageWidth << std::dec << std::endl;
    std::cout << "  Image Height: " << loadedImageHeight << std::dec << std::endl;
    std::cout << "  Image Channels: " << loadedImageChannels << std::dec << std::endl;

    unsigned char *ghostFile = ImageProcessing::load_image_from_memory(ghostFileBuf, fileSize, &loadedImageWidth, &loadedImageHeight, &loadedImageChannels);
    std::cout << "✅ Loaded Image!" << std::endl;
    std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;

    std::cout << "↔️ Resizing Image..." << std::endl;
    int newWidth = NUM_LEDS_X;
    int newHeight = NUM_LEDS_Y;
    unsigned char *resizedGhost = ImageProcessing::resize_image(ghostFile, loadedImageWidth, loadedImageHeight, loadedImageChannels, newWidth, newHeight, true);
    std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;
    std::cout << "🕊️ Freeing Loaded Image..." << std::endl;
    std::cout << "🕊️ Freeing Image & Buffer..." << std::endl;
    ImageProcessing::free_image(ghostFile);
    free(ghostFileBuf);

    // std::cout << "⏳ Loading Show..." << std::endl;
    // File showFile = fm->getJsonFile("/show.json");
    // show = loadShow(showFile);
    // std::cout << "✅ Loaded Show!" << std::endl;
    // std::cout << "➡️ Show Name: " << show.name << std::endl;
    // showFile.close();

    // DEBUG: Display the loaded image data
    std::cout << "🖼️ Displaying Image Data..." << std::endl;
    std::cout << "  New Width: " << newWidth << std::endl;
    std::cout << "  New Height: " << newHeight << std::endl;
    ImageProcessing::printImageHex(resizedGhost, newWidth, newHeight, loadedImageChannels);

    // Load the resized image into the LED matrix
    std::cout << "🚦 Loading Image into LED Array..." << std::endl;
    CRGB *serpentineArray = (CRGB*)calloc(NUM_LEDS, sizeof(CRGB));
    bufferToCRGBArray(resizedGhost, newWidth, newHeight, loadedImageChannels, serpentineArray, NUM_LEDS_X, NUM_LEDS_Y);
    fillRemainingPixels(serpentineArray, NUM_LEDS_X, NUM_LEDS_Y, CRGB::DarkOliveGreen);

    rearrangeForSerpentine(serpentineArray, leds, NUM_LEDS_X, NUM_LEDS_Y);
    free(serpentineArray);

    // Log the time taken to load the ghost
    long end_loading_ghost = millis();
    std::cout << "⏲️ Time taken to load ghost: " << end_loading_ghost - start_loading_ghost << "ms" << std::endl;

    FastLED.show();

    // TODO: `free` or free_image causes a kernel panic, possibly due to a double free, the large size, or something else.
    //  Not freeing will cause a memory leak, but it's better than a kernel panic for now.
    // std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;
    // std::cout << "🕊️ Freeing Loaded Image..." << std::endl;
    // free(resizedGhost);
    // std::cout << "✅ Freed Image!" << std::endl;

    std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;
    std::cout << "🛑 Closing ghost from file manager..." << std::endl;
    ghostBmpFile.close();
    std::cout << "✅ Closed ghost from file manager!" << std::endl;
}

/**
 * MARK: Looping
 */
void loop()
{
    // gets the current state of every sensor, the state automatically resets after it's viewed
    auto current_millis = millis();
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

/**
 * DOESN'T WORK. DUNNO WHY!
 */
void loadByteBitmap(CRGB *leds, const unsigned char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth)
{
    for (uint8_t y = 0; y < bitmapHeight; y++)
    {
        for (uint8_t x = 0; x < bitmapWidth; x++)
        {
            // Only draw if within bounds of the LED matrix
            if ((startX + x) < kMatrixWidth && (startY + y) < kMatrixHeight)
            {
                // Calculate the index for this pixel (3 bytes per pixel: RGB)
                int index = (y * bitmapWidth + x) * 3;

                // Extract RGB values from the hex array
                char hexColor[7];
                snprintf(hexColor, sizeof(hexColor), "%02x%02x%02x", bitmap[index], bitmap[index + 1], bitmap[index + 2]);
                hexColor[6] = '\0';

                if (startX + x < kMatrixHeight && startY + y < kMatrixHeight)
                {
                    CRGB color = hexToCRGB(hexColor);
                    if (color != (CRGB::Black))
                    {
                        leds[XY(startX + x, startY + y)] = color;
                    }
                }
            }
        }
    }
    FastLED.show(); // Display the updated LED matrix
}

void shiftLeds(CRGB leds[], ShiftDirection direction)
{
    CRGB temp[NUM_LEDS_X * NUM_LEDS_Y];

    // Copy current state to temp array
    for (int i = 0; i < NUM_LEDS_X * NUM_LEDS_Y; i++)
    {
        temp[i] = leds[i];
    }

    switch (direction)
    {
    case RIGHT: // if you look closely right and left might look flipped, and you're right!
                // don't ask me why, it just works :)
        for (int y = 0; y < NUM_LEDS_Y; y++)
        {
            for (int x = 0; x < NUM_LEDS_X; x++)
            {
                int newX = (x - 1 + NUM_LEDS_X) % NUM_LEDS_X;
                leds[XY(newX, y)] = temp[XY(x, y)];
            }
        }
        break;

    case LEFT:
        for (int y = 0; y < NUM_LEDS_Y; y++)
        {
            for (int x = 0; x < NUM_LEDS_X; x++)
            {
                int newX = (x + 1) % NUM_LEDS_X;
                leds[XY(newX, y)] = temp[XY(x, y)];
            }
        }
        break;

    case UP:
        for (int x = 0; x < NUM_LEDS_X; x++)
        {
            for (int y = 0; y < NUM_LEDS_Y; y++)
            {
                int newY = (y - 1 + NUM_LEDS_Y) % NUM_LEDS_Y;
                leds[XY(x, newY)] = temp[XY(x, y)];
            }
        }
        break;

    case DOWN:
        for (int x = 0; x < NUM_LEDS_X; x++)
        {
            for (int y = 0; y < NUM_LEDS_Y; y++)
            {
                int newY = (y + 1) % NUM_LEDS_Y;
                leds[XY(x, newY)] = temp[XY(x, y)];
            }
        }
        break;
    }

    FastLED.show(); // Update the LED display
}

/**
 * duration is given in seconds.
 *
 * Uses FastLED's builtin for setting brightness, so it'll modify every LED.
 *
 * If you want to use it on a specific subset of LEDs, will have to provide
 * that subset as well as somehow keeping track of what the LEDs previously were.
 */
void fadeToBlack(int duration)
{
    uint8_t initialBrightness = FastLED.getBrightness();
    if (initialBrightness == 0)
        return;

    int updatesPerSec = initialBrightness / duration;

    for (int i = initialBrightness; i > 0; i--)
    {
        FastLED.setBrightness(i);
        FastLED.show();
        delay(1000 / updatesPerSec);
    }

    // Ensure the brightness is fully set to 0 at the end
    FastLED.setBrightness(0);
    FastLED.show();
}

/**
 * Slowly and "smoothly" transitions the brightness of all LEDs to the targetBrightness over the given duration.
 *
 * duration is given in seconds.
 * targetBrightness should generally not be set beyond 32 (64 likely maximum for safety/consistent power delivery)
 *
 * Uses FastLED's builtin for setting brightness, so it'll modify every LED.
 *
 * If you want to use it on a specific subset of LEDs, will have to provide
 * that subset as well as somehow keeping track of what the LEDs previously were.
 */
void fadeToBrightness(int duration, int targetBrightness)
{
    uint8_t curBrightness = FastLED.getBrightness();
    if (curBrightness == targetBrightness)
        return;

    int updatesPerSec;
    if (targetBrightness > curBrightness)
    { // increase brightness to target
        updatesPerSec = (targetBrightness - curBrightness) / duration;
        for (int i = curBrightness; i < targetBrightness; i++)
        {
            FastLED.setBrightness(i);
            FastLED.show();
            delay(1000 / updatesPerSec);
        }
    }
    else
    { // decrease brightness to target
        updatesPerSec = (curBrightness - targetBrightness) / duration;
        for (int i = curBrightness; i > targetBrightness; i--)
        {
            FastLED.setBrightness(i);
            FastLED.show();
            delay(1000 / updatesPerSec);
        }
    }

    FastLED.setBrightness(targetBrightness); // just in case it doesnt fully work lol
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

// Function to parse the bitmap data from a hex string
void parseBitmapData(const char *hexData)
{
    int index = 0;
    while (*hexData)
    {
        // Skip spaces
        if (*hexData == ' ')
        {
            hexData++;
            continue;
        }

        // Convert the next 6 characters to CRGB and store in the leds array
        if (index < NUM_LEDS)
        {
            leds[index] = hexToCRGB(hexData);
            hexData += 6; // Move to the next color
            index++;
        }
        else
        {
            break; // Avoid exceeding the array size
        }
    }
}

/**
 * Draws a single frame of the rainbow effect
 */
void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
    uint8_t lineStartHue = startHue8;
    for (uint8_t y = 0; y < kMatrixHeight; y++)
    {
        lineStartHue += yHueDelta8;
        uint8_t pixelHue = lineStartHue;
        for (uint8_t x = 0; x < kMatrixWidth; x++)
        {
            pixelHue += xHueDelta8;
            leds[XY(x, y)] = CHSV(pixelHue, 255, 255);
        }
    }
}

/**
 * Draws a single frame of the rainbow effect
 */
void DrawOneFrameReducedBright(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
    uint8_t lineStartHue = startHue8;
    for (uint8_t y = 0; y < kMatrixHeight; y++)
    {
        lineStartHue += yHueDelta8;
        uint8_t pixelHue = lineStartHue;
        for (uint8_t x = 0; x < kMatrixWidth; x++)
        {
            pixelHue += xHueDelta8;
            leds[XY(x, y)] = CHSV(pixelHue * 5 / 6, 255 * 5 / 6, 255 * 5 / 6);
        }
    }
}

void bufferToCRGBArray(unsigned char* buffer, int imgWidth, int imgHeight, int imgChannels, CRGB* leds, int matrixWidth, int matrixHeight) {
    int startX = (matrixWidth - imgWidth) / 2;
    int startY = (matrixHeight - imgHeight) / 2;

    for (int y = 0; y < imgHeight; ++y) {
        for (int x = 0; x < imgWidth; ++x) {
            int bufferIndex = (y * imgWidth + x) * imgChannels;
            int matrixIndex = (startY + y) * matrixWidth + (startX + x);

            if (imgChannels == 3) { // RGB
                leds[matrixIndex] = CRGB(buffer[bufferIndex], buffer[bufferIndex + 1], buffer[bufferIndex + 2]);
            } else if (imgChannels == 4) { // RGBA
                leds[matrixIndex] = CRGB(buffer[bufferIndex], buffer[bufferIndex + 1], buffer[bufferIndex + 2]);
            }
        }
    }
}

void fillRemainingPixels(CRGB* leds, int matrixWidth, int matrixHeight, CRGB backgroundColor) {
    for (int y = 0; y < matrixHeight; ++y) {
        for (int x = 0; x < matrixWidth; ++x) {
            int index = y * matrixWidth + x;
            if (leds[index] == CRGB::Black) {
                leds[index] = backgroundColor;
            }
        }
    }
}
#endif

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

