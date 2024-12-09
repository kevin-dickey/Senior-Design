#include <iostream>
#include <chrono>
#include "json.hpp"
using json = nlohmann::json;

#include <FileManager.h>

#define LED_PIN 13
// #define NUM_LEDS_X 16
// #define NUM_LEDS_Y 16
// #define NUM_LEDS 256
#define MAX_BRIGHTNESS 64 // maximum for FastLED is 255, (don't go higher than like 8 if you don't have a PSU attached)

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
#include "Configuration.h"
#include "ControllerRunner.h"
#include "Sensor.h"
#include "ControllerRunner.h"
#include <ImageProcessing.h>
#include "rippleEffect.h"
#include "utils.h"

#endif


enum ShiftDirection {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

namespace std {
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&...args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace std



unsigned long getMillis() {
#if USE_EMULATOR
    auto now = std::chrono::high_resolution_clock ::now();
    auto mseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - epoch).count();
    return mseconds;
#else
    return millis();
#endif
}

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

    showStart = getMillis();
    runner = new ControllerRunner(show, showStart, epoch);
    auto sensor_state = std::vector<bool>{false, false, false, false};

    for (int i = 0; i < 30; i++)
    {
        if (i == 11)
        {
            sensor_state = std::vector<bool>{false, false, false, false};
        }
        if (i == 10)
        {
            // sensorValues = sensorManager->getSensors()
            sensor_state = std::vector<bool>{true, false, false, false};
        }
        if ((show.duration <= runner->getEffectCursor()))
        {
            delete runner;
            runner = nullptr;
            showStart = getMillis();
            epoch = std::chrono::high_resolution_clock::from_time_t(0);
            runner = new ControllerRunner(show, showStart, epoch);
        }

        auto showFrame = runner->getNextShowFrame(sensor_state);
        if (showFrame.effect->name != "no effect" && showFrame.frame != -1)
        {
            std::cout << "Effect: " << showFrame.effect->name << std::endl;
            std::cout << "Frame: " << showFrame.frame << std::endl;
        }
        else
        {
            std::cout << "no effect found. resetting runner " << std::endl;
            delete runner;
            runner = nullptr;
            showStart = getMillis();
            epoch = std::chrono::high_resolution_clock::from_time_t(0);
            runner = new ControllerRunner(show, showStart, epoch);
        }
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
 * MARK: Prototypes
 */
void fadeToBlack(int duration);
void fadeToBrightness(int duration, int targetBrightness);
void drawRainbow(unsigned long current_millis);
// void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8);           // used in drawRainbow
void DrawOneFrameReducedBright(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8); // ^ @ slightly lower brightness for readability of other things on leds (e.g. pumpkin more easily visible)
void parseBitmapData(const char *hexData);
uint8_t bufferToCRGBArray(unsigned char* buffer, int imgWidth, int imgHeight, int imgChannels, CRGB* leds, int matrixWidth, int matrixHeight, int startX, int startY, bool wrap = false);
void loadImagesFromSD(std::vector<std::string> images);
void fillRemainingPixels(CRGB *leds, int matrixWidth, int matrixHeight, CRGB backgroundColor);
void loadHexBitmap(CRGB *leds, const char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth);
CRGB hexToCRGB(const char *hex);
void resetTriggerMarkers(int exception);
void newEffectReset();
void generateFrame(ControllerRunner::ShowFrame showframe);
void shiftLeds(CRGB leds[], ShiftDirection direction);

// MARK: Variables
// led stuff
CRGB* leds;
int NUM_LEDS = 256;
int NUM_LEDS_X = 16;
int NUM_LEDS_Y = 16;
int LEDS_SIZE_ARR[2] = {NUM_LEDS_X, NUM_LEDS_Y};
uint8_t kMatrixWidth;
uint8_t kMatrixHeight;

// ripple effect stuff
int* prevLeds1;
int* prevLeds2;
int* prevLeds3;
int* prevLeds4;
int rippleCounter;

// sensor & show stuff
SensorManager *sensorManager;
ControllerRunner *runner;
unsigned long showStart = 0;
std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 1000000000> > > epoch;
Sensor *sensor0, *sensor1, *sensor2, *sensor3;
std::vector<Sensor *> a_sensors;
std::vector<bool> prev_sensor_triggered;

// misc. stuff (effect variables, sensor stuff, running-time)
int hue;
int curEffect = -1;
int count;
bool goUp;
bool loaded = false;
char set_sensors;
unsigned long current_millis;

FileManager *fm = NULL;

// can be loaded using loadHexBitmap
// const char *pumpkin = "ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 74401f ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 764322 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ff7f15 ff7f15 ff7f15 80431c 74401f ff7f15 ff7f15 ff7f15 ff8017 ffffff ffffff ffffff ffffff ffffff ffffff ff7f15 ff7f15 ff8c2c ff7f15 ff7f15 ff8c2c ff7f15 ff7f15 ff7f15 ff7f15 ff8623 ffffff ffffff ffffff ffffff ff7f15 de741d ff8c2c 2d1200 2d1200 ff7f15 ff7f15 2d1200 2d1200 ff8c2c f77b15 ff8723 ffffff ffffff ffffff ff8118 ff7f15 ff8c2c 2d1200 863b20 863b20 2d1200 69320a 863b20 863b20 2d1200 ff8118 ff7f15 ff8520 ffffff ffffff ff7f15 ff7f15 ff7f15 ff7f15 db731d ff7f15 ff7f15 ff7f15 ff8c2c ff7f15 ff7f15 ff8c2c ff7f15 ff8c2c ffffff ffffff ff7f15 ff7f15 ff7f15 ff7f15 f27914 ff7f15 2c1100 2d1200 ff8c2c ff7f15 ff7f15 ff8c2c ff7f15 ff8c2c ffffff ffffff ff7f15 ff7f15 863b20 ff7f15 f27914 ff7f15 ff7f15 ff7f15 ff8c2c f27914 2d1200 863b20 ff7f15 ff8c2c ffffff ffffff ff7f15 ff7f15 ff7f15 2a1000 f27914 2d1200 2d1200 2d1200 2d1200 f27914 2d1200 ff7f15 ff7f15 ff7f15 ffffff ffffff ff7f15 ff7f15 ff7f15 863b20 2d1200 2d1200 2d1200 2d1200 2d1200 2d1200 863b20 cf7022 ff7f15 ffffff ffffff ffffff ffffff ff7f15 f27914 ff7f15 863b20 2d1200 2d1200 2d1200 2d1200 863b20 ff7f15 f27914 ff7f15 ffffff ffffff ffffff ffffff ffffff ff7f15 f27914 f67a14 f27914 f27914 f27914 cc7024 f27914 dc731d ff7f15 ffffff ffffff ffffff ffffff ffffff ffffff ffffff f27914 ed7816 f27914 f27914 f27914 f27914 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff";
// const char *pumpkin8bit = "000000 000000 000000 000000 267f00 267f00 000000 000000 000000 000000 000000 267f00 267f00 000000 000000 000000 000000 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 000000 ff6100 ff6100 ffec1e ff6100 ff6100 ffec1e ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 ffec1e ff6100 ff6100 ff6100 ff6100 ffec1e ff6100 ff6100 ff6100 ffec1e ffec1e ffec1e ffec1e ff6100 ff6100 000000 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 000000";
// const char *ghost8bit = "000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff 000000 ffffff ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff ffffff 000000 ffffff 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff ffffff 000000 000000 ffffff ffffff 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff 000000 ffffff ffffff 000000 ffffff 000000";
// const char *skull8bit = "000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000 ffffff ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff ffffff 000000 000000 000000 000000 000000 000000 000000 ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff 000000 000000 000000 000000 000000 000000 000000 000000 ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff 000000 000000 000000 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000";
unsigned char *ghostjpg;
unsigned char *pumpkinjpg;
unsigned char *candyCanejpg;
unsigned char *christmasTreejpg;
unsigned char *snowflakejpg;
unsigned char *snowmanjpg;


unsigned char bufferPattern[3][3][3] = {
    {{0x00, 0x00, 0xFF}, {0x00, 0x00, 0xFF}, {0x00, 0xFF, 0x00}},
    {{0x00, 0x00, 0xFF}, {0x00, 0xFF, 0x00}, {0xFF, 0xFF, 0xFF}},
    {{0x00, 0xFF, 0x00}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}}};

// Pointer to the array
unsigned char *bufferPtr = &bufferPattern[0][0][0];

int loadedImageHeight, loadedImageWidth, loadedImageChannels;
unsigned char *loadedImage;

// MARK: Setup
void setup()
{
    Serial.begin(115200);                    // for setting up stuff to print to serial monitor
    delay(3000);                             // delay for 3 seconds to give time to open the serial monitor
    std::cout << "Starting..." << std::endl; // print to the serial monitor that the program is starting

    // SD Card Setup
    fm = new FileManager();
    if (!fm->MountFileSystem())
    {
        std::cout << "Failed to mount file system. Halting..." << std::endl;
        while (true)
            ;
    }

    std::cout << " we mounted " << ESP.getFreeHeap() << std::endl;

    try {
        std::cout << "⏳ Loading Show..." << std::endl;
        File showFile = fm->getJsonFile("/christmas-y.json");
        show = loadShow(showFile);
        std::cout << "✅ Loaded Show!" << std::endl;
        // std::cout << "➡️ Show Name: " << show.name << std::endl;
        runner = new ControllerRunner(show, millis(), epoch);
        showFile.close();
    } catch (const std::exception &e) {
        std::cerr << "Error loading show: " << e.what() << std::endl;
    }

    if (show.layouts[0]->isGridLayout()) {          // might need to be a try catch instead (isGridLayout not defined for other types, but other types also not rlly defined afaict)
        NUM_LEDS_X = show.layouts[0]->getWidth();   // this returns size of frontend, SHOULD be 100
        NUM_LEDS_Y = show.layouts[0]->getHeight();  // SHOULD be 24
        NUM_LEDS = NUM_LEDS_X * NUM_LEDS_Y;         // SHOULD be 2400
        kMatrixHeight = NUM_LEDS_Y;
        kMatrixWidth = NUM_LEDS_X;

        leds = new CRGB[NUM_LEDS];

        // waiting on confirm if you want to double the computational intensity for ripple effect in lieu of saving on storage
        prevLeds1 = new int[NUM_LEDS];
    } else {
        // custom layout, not gonna bother with this rn but you'll have to set the same variables in some way (there aren't height and width params passed)
    }

    // FastLED Initialization
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050); // setup the LEDs & LED pin for the esp32
    FastLED.setBrightness(MAX_BRIGHTNESS);                                                        // set the max brightness for the LEDs
    pinMode(LED_BUILTIN, OUTPUT);                                                                 // setup the built-in LED for the esp32
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    Serial.println("Initialized FastLED...");
    std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;

    // sensor stuff, might need some updating idk
    Pair_t sensor_pos{0, 0};
    Pair_t size = {10, 10};
    std::unique_ptr<Translation_t> translation = std::make_unique<Translation_t>(
        Translation_t{
            .end_pos = sensor_pos,
            .durationMs = 1000.0});
    Effect *sensorEffect = new Effect(1, rainbow, "BasicEffect", sensor_pos, size, 0.0, 500.0, std::move(translation));
    // TODO: Figure out the 0-indexing. Need to have same behavior on both sides
    // sensor0 = new Sensor(0, 34, 1000, S_BINARY, sensor_pos, sensorEffect);  // ne
    sensor1 = new Sensor(1, 35, 1000, S_BINARY, sensor_pos, sensorEffect);  // nw
    // sensor2 = new Sensor(2, 32, 1000, S_BINARY, sensor_pos, sensorEffect);  // sw
    // sensor3 = new Sensor(3, 33, 1000, S_BINARY, sensor_pos, sensorEffect);  // se
    a_sensors = std::vector<Sensor *>{sensor1};
    prev_sensor_triggered = std::vector<bool>(a_sensors.size());
    sensorManager = new SensorManager();
    sensorManager->setSensors(a_sensors);
    set_sensors = 'a';
    Serial.println("Initialized Sensors...");

    hue = 30;
    count = 0;
    rippleCounter = 0;
    goUp = true;
    srand(static_cast<unsigned int>(time(0)));

    // for (effect which uses an image : show) {
    //     append necessary filepaths to a std::vector<std::string> or whatever datatype you want
    // }
    // loadImagesFromSD(std::vector<std::string>); // call this and we'll be good
    std::vector<std::string> imgs = {"/8bitpumpkin.jpg", "/8bitghost.jpg"};
    loadImagesFromSD(imgs);
    std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;
    std::cout << pumpkinjpg << std::endl;
    std::cout << ghostjpg << std::endl;
}


/**
 * MARK: Looping
 */
void loop()
{
    std::cout << " here!! " << std::endl;
    // gets the current state of every sensor, the state automatically resets after it's viewed
    auto sensor_states = sensorManager->getSensorStates(true);
    std::cout << " no here!! " << std::endl;
    current_millis = millis();
    std::cout << " here...? " << std::endl;

    // sets the active sensors so that the runner is constanly checking sensor state and determining what to display
    // get showFrame
    auto showFrame = runner->getNextShowFrame(sensor_states);
    std::cout << " wahhh!!! " << showFrame.effect->name << showFrame.frame << std::endl;
    while (showFrame.effect->name == "no effect" && showFrame.frame == -1)
    {
        std::cout << " skibidi " << std::endl;
        // no effect found because we're over the shows duration. reset show
        std::string filePath = "C:/Users/Eleen/Desktop/Senior Design/sddec24-15/sddec24-15/PixelController/data/show.json";
        Show show = loadShow(filePath);
        showStart = getMillis();
        epoch = std::chrono::high_resolution_clock::from_time_t(0);
        runner = new ControllerRunner(show, showStart, epoch);
        showFrame = runner->getNextShowFrame(sensor_states);
    }

    std::cout << " FRAME TIME!!! " << std::endl;
    generateFrame(showFrame);

    // divide frame up to send to picos


    delay(33);  // delay(33): approx 30fps (30.3)
}


// MARK: Functions


/**
 * Loads in images from the SD card, images should be a string of the filepath (e.g. "/djibouti.jpg")
 */
void loadImagesFromSD(std::vector<std::string> images) {
    for (const std::string &image : images) {
        try {
            std::cout << "⏳ Loading " << image << "..." << std::endl;
            File jpgFile = fm->getJsonFile(image);
            std::cout << "✅ Opened " << image << " File!" << std::endl;

            std::cout << "Converting to FILE..." << std::endl;
            size_t fileSize;
            unsigned char *fileBuf = ImageProcessing::convertFsFileToBuffer(&jpgFile, fileSize);  // <--
            std::cout << "✅ Converted to FILE!" << std::endl;
            std::cout << "  File Size: " << fileSize << std::endl;
            std::cout << "  First 10 bytes: " << fileBuf[0] << fileBuf[1] << fileBuf[2] << fileBuf[3] << fileBuf[4] << fileBuf[5] << fileBuf[6] << fileBuf[7] << fileBuf[8] << fileBuf[9] << std::endl;

            std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;
            std::cout << "🛑 Closing ghost from file manager..." << std::endl;
            jpgFile.close();
            std::cout << "✅ Closed ghost from file manager!" << std::endl;

            std::cout << "⏳ Getting Image Dimensions..." << std::endl;

            // Get the dimensions of the image and load it
            ImageProcessing::get_image_dimensions_from_memory(fileBuf, fileSize, &loadedImageWidth, &loadedImageHeight, &loadedImageChannels);
            std::cout << "  Image Width: " << loadedImageWidth << std::dec << std::endl;
            std::cout << "  Image Height: " << loadedImageHeight << std::dec << std::endl;
            std::cout << "  Image Channels: " << loadedImageChannels << std::dec << std::endl;

            unsigned char *imageFile = ImageProcessing::load_image_from_memory(fileBuf, fileSize, &loadedImageWidth, &loadedImageHeight, &loadedImageChannels);
            std::cout << "✅ Loaded Image!" << std::endl;
            std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;

            
            // making it always resize for now

            // hard coding the resize to be 24x25, given the field is 24x100. for future fields just always resize img to what user wants/how many times they want it repeated
            // resizing only ghost and pumpkin (or unrecognized file), might just want to recreate them to be right dimensions tbh
            // if (image == "/8bitghost.jpg" || "/8bitpumpkin.jpg" || !("/candycane.jpg" || "/snowflake.jpg" || "/christmastree.jpg" || "/snowman.jpg")) {
                std::cout << "↔️ Resizing " << image << " Image..." << std::endl;
                int newWidth = NUM_LEDS_X / 4;
                int newHeight = NUM_LEDS_Y;
                imageFile = ImageProcessing::resize_image(imageFile, loadedImageWidth, loadedImageHeight, loadedImageChannels, newWidth, newHeight, true);
            // }

            std::cout << "🕊️ Freeing File Buffer..." << std::endl;
            free(fileBuf);

            // DEBUG: Print the loaded image data
            std::cout << "🖼️ Displaying Image Data..." << std::endl;
            ImageProcessing::printImageHex(imageFile, 24, 25, loadedImageChannels);

            // Store the image
            std::cout << "💾 Storing image to the global pointer..." << std::endl;
            if (image == "/8bitghost.jpg") {
                ghostjpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            } else if (image == "/8bitpumpkin.jpg") {
                pumpkinjpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            } else if (image == "/candycane.jpg") {
                candyCanejpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            } else if (image == "/snowflake.jpg") {
                snowflakejpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            } else if (image == "/christmastree.jpg") {
                christmasTreejpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            } else if (image == "/snowman.jpg") {
                snowmanjpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            } else {
                std::cerr << "Unrecognized file supplied (should be a jpg): '" << image << "' !" << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error loading image: " << e.what() << std::endl;
        }
    }
    std::cout << "😁 Done loading images!" << std::endl;
}


/**
 * Builds frame onto the leds array (CRGB leds[]) based on the provided showframe.
 * 
 * Limited -- only generates frames based on the specified EffectTypes in the backend (Effect.h),
 *         -- which is based on the frontend EffectTypes, but the backend needs to be manually updated to
 *         -- whatever EffectTypes the frontend has.
 */
void generateFrame(ControllerRunner::ShowFrame showframe) {
    // shift directions might be incorrect, need to test on field!
    // rainbow + shifting effects might look a little goofy, but shouldn't be completely broken i don't think
    // ripple + shifting effects almost surely broken af
    switch (showframe.effect->effectType) {  // set-up the leds[] with the frame based on desired effect
        case rainbow:
            std::cout << "Generating rainbow frame..." << std::endl;
            if (curEffect != 0) {
                newEffectReset;
                curEffect = 0;
            }

            drawRainbow(current_millis);
            FastLED.show();

            break;

        case ripple:
            std::cout << "Generating ripple frame..." << std::endl;
            if (curEffect != 1) {
                newEffectReset;
                curEffect = 1;
            }

            rippleEffect(leds, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter, prevLeds1, 2);
            FastLED.show();

            break;

        case pumpkinRainbow:
            std::cout << "Generating pumpkinRainbow frame..." << std::endl;
            if (curEffect != 2) {
                newEffectReset;
                curEffect = 2;
            }

            // draw rainbow with pumpkin on top
            drawRainbow(current_millis);

            // load in pumpkins (check locations are good)
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 25, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 50, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 75, 0, true);

            shiftLeds(leds, RIGHT); // might make the rainbow effect look v weird, not sure
            break;

        case pumpkinRipple:
            std::cout << "Generating pumpkinRipple frame..." << std::endl;
            if (curEffect != 3) {
                newEffectReset;
                curEffect = 3;
            }

            // draw ripple with pumpkin on top
            rippleEffect(leds, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter, prevLeds1, 2);

            // (check locations are good)
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 25, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 50, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 75, 0, true);

            FastLED.show();
            break;

        case ghostRainbow:
            std::cout << "Generating ghostRainbow frame..." << std::endl;
            if (curEffect != 4) {
                newEffectReset;
                curEffect = 4;
            }

            // draw rainbow with ghost on top  
            drawRainbow(current_millis);

            // (check locations are good)
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 25, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 50, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 75, 0, true);

            shiftLeds(leds, RIGHT); // calls FastLED.show()
            break;

        case ghostRipple:
            std::cout << "Generating ghostRipple frame..." << std::endl;
            if (curEffect != 5) {
                newEffectReset;
                curEffect = 5;
            }

            // draw ripple with ghost on top
            rippleEffect(leds, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter, prevLeds1, 2);

            // (check locations are good)
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 25, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 50, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 75, 0, true);

            FastLED.show();
            break;

        case pumpkinGhostRainbow:
            std::cout << "Generating pumpkinGhostRainbow frame..." << std::endl;
            if (curEffect != 6) {
                newEffectReset;
                curEffect = 6;
            }

            // draw rainbow, then pumpkin and ghost
            drawRainbow(current_millis);

            // need to set locations
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);            

            shiftLeds(leds, RIGHT); // calls FastLED.show()
            break;

        case pumpkinGhostRipple:
            std::cout << "Generating pumpkinGhostRipple frame..." << std::endl;
            if (curEffect != 7) {
                newEffectReset;
                curEffect = 7;
            }

            // draw ripple, then pumpkin and ghost
            rippleEffect(leds, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter, prevLeds1, 2);

            // (check locations are good)
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 25, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 50, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 75, 0, true);

            // shiftLeds(leds, RIGHT); // calls FastLED.show()
            FastLED.show();
            break;

        case snowflake:
            std::cout << "Generating snowflake frame..." << std::endl;
            if (curEffect != 8) {
                newEffectReset;
                curEffect = 8;
            }

            if (!loaded) {
                // need to set locations
                bufferToCRGBArray(snowflakejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
                bufferToCRGBArray(snowflakejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 25, 0, true);
                bufferToCRGBArray(snowflakejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 50, 0, true);
                bufferToCRGBArray(snowflakejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 75, 0, true);
                loaded = true;
            }

            shiftLeds(leds, DOWN); // calls FastLED.show()
            break;

        case snowman:
            std::cout << "Generating snowman frame..." << std::endl;
            if (curEffect != 9) {
                newEffectReset;
                curEffect = 9;
            }

            if (!loaded) {
                // (check locations are good)
                bufferToCRGBArray(snowmanjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
                bufferToCRGBArray(snowmanjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 25, 0, true);
                bufferToCRGBArray(snowmanjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 50, 0, true);
                bufferToCRGBArray(snowmanjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 75, 0, true);
                loaded = true;
            }

            shiftLeds(leds, RIGHT); // calls FastLED.show()
            break;

        case christmasTree:
            std::cout << "Generating christmasTree frame..." << std::endl;
            if (curEffect != 10) {
                newEffectReset;
                curEffect = 10;
            }

            if (!loaded) {
                // load in christmas tree & pattern onto leds
                bufferToCRGBArray(christmasTreejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
                bufferToCRGBArray(christmasTreejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 25, 0, true);
                bufferToCRGBArray(christmasTreejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 50, 0, true);
                bufferToCRGBArray(christmasTreejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 75, 0, true);
                loaded = true;
            }

            shiftLeds(leds, RIGHT); // calls FastLED.show()
            break;

        case candyCane:
            std::cout << "Generating candyCane frame..." << std::endl;
            if (curEffect != 11) {
                newEffectReset;
                curEffect = 11;
            }

            if (!loaded) {
                // need to set locations!
                bufferToCRGBArray(candyCanejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 0, 0, true);
                bufferToCRGBArray(candyCanejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 25, 0, true);
                bufferToCRGBArray(candyCanejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 50, 0, true);
                bufferToCRGBArray(candyCanejpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, 75, 0, true);
                loaded = true;
            }

            shiftLeds(leds, DOWN); // calls FastLED.show()
            break;

        default:
            newEffectReset();
            Serial.println("  !Error! Effect not found/recognized (likely need to update Effect.h to match the effects on frontend).");
            return;
    }
}

// Resets some variables and such for the next effect to run properly
void newEffectReset() {
    loaded = false;
    prevLeds1 = {0};
    rippleCounter = 0;
    curEffect = -1;

    // might be used? no harm in resetting if not
    count = 0;
    goUp = true;
}


// current_millis isn't a LiveData (see android studio), so should be ok to pass (don't need to though, it's relatively global)
void drawRainbow(unsigned long current_millis) {
    int32_t yHueDelta32 = ((int32_t)cos16(current_millis * (27 / 1)) * (350 / kMatrixWidth));
    int32_t xHueDelta32 = ((int32_t)cos16(current_millis * (39 / 1)) * (310 / kMatrixHeight));
    DrawOneFrameReducedBright(current_millis / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
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

// cant remember if these were moved somewhere else in main, keepin here will remove if nedded
void shiftLeds(CRGB leds[], ShiftDirection direction)
{
    CRGB temp[NUM_LEDS_X * NUM_LEDS_Y];

    // Copy current state to temp array
    for (int i = 0; i < NUM_LEDS_X * NUM_LEDS_Y; i++) {
        temp[i] = leds[i];
    }

    switch (direction) {
        case RIGHT:  // if you look closely right and left might look flipped, and you're right!
                     // don't ask me why, it just works :)
            for (int y = 0; y < NUM_LEDS_Y; y++) {
                for (int x = 0; x < NUM_LEDS_X; x++) {
                    int newX = (x - 1 + NUM_LEDS_X) % NUM_LEDS_X;
                    leds[XY(newX, y)] = temp[XY(x, y)];
                }
            }
            break;

        case LEFT:
            for (int y = 0; y < NUM_LEDS_Y; y++) {
                for (int x = 0; x < NUM_LEDS_X; x++) {
                    int newX = (x + 1) % NUM_LEDS_X;
                    leds[XY(newX, y)] = temp[XY(x, y)];
                }
            }
            break;

        case UP:
            for (int x = 0; x < NUM_LEDS_X; x++) {
                for (int y = 0; y < NUM_LEDS_Y; y++) {
                    int newY = (y - 1 + NUM_LEDS_Y) % NUM_LEDS_Y;
                    leds[XY(x, newY)] = temp[XY(x, y)];
                }
            }
            break;

        case DOWN:
            for (int x = 0; x < NUM_LEDS_X; x++) {
                for (int y = 0; y < NUM_LEDS_Y; y++) {
                    int newY = (y + 1) % NUM_LEDS_Y;
                    leds[XY(x, newY)] = temp[XY(x, y)];
                }
            }
            break;
    }

    FastLED.show();  // Update the LED display
}


/**
 * duration is given in seconds.
 *
 * Uses FastLED's builtin for setting brightness, so it'll modify every LED.
 *
 * If you want to use it on a specific subset of LEDs, will have to provide
 * that subset as well as somehow keeping track of what the LEDs previously were.
 */
void fadeToBlack(int duration) {
    uint8_t initialBrightness = FastLED.getBrightness();
    if (initialBrightness == 0)
        return;

    int updatesPerSec = initialBrightness / duration;

    for (int i = initialBrightness; i > 0; i--) {
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
void fadeToBrightness(int duration, int targetBrightness) {
    uint8_t curBrightness = FastLED.getBrightness();
    if (curBrightness == targetBrightness)
        return;

    int updatesPerSec;
    if (targetBrightness > curBrightness) {  // increase brightness to target
        updatesPerSec = (targetBrightness - curBrightness) / duration;
        for (int i = curBrightness; i < targetBrightness; i++) {
            FastLED.setBrightness(i);
            FastLED.show();
            delay(1000 / updatesPerSec);
        }
    } else {  // decrease brightness to target
        updatesPerSec = (curBrightness - targetBrightness) / duration;
        for (int i = curBrightness; i > targetBrightness; i--) {
            FastLED.setBrightness(i);
            FastLED.show();
            delay(1000 / updatesPerSec);
        }
    }

    FastLED.setBrightness(targetBrightness);  // just in case it doesnt fully work lol
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

/**
 * Draws a single frame of the rainbow effect
 */
void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
    uint8_t lineStartHue = startHue8;
    for (uint8_t y = 0; y < kMatrixHeight; y++) {
        lineStartHue += yHueDelta8;
        uint8_t pixelHue = lineStartHue;
        for (uint8_t x = 0; x < kMatrixWidth; x++) {
            pixelHue += xHueDelta8;
            leds[XY(x, y)] = CHSV(pixelHue, 255, 255);
        }
    }
}


/**
 * Draws a single frame of the rainbow effect @ reduced brightness
 */
void DrawOneFrameReducedBright(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
    uint8_t lineStartHue = startHue8;
    for (uint8_t y = 0; y < kMatrixHeight; y++) {
        lineStartHue += yHueDelta8;
        uint8_t pixelHue = lineStartHue;
        for (uint8_t x = 0; x < kMatrixWidth; x++) {
            pixelHue += xHueDelta8;
            leds[XY(x, y)] = CHSV(pixelHue * 5 / 6, 255 * 5 / 6, 255 * 5 / 6);
        }
    }
}

/**
 * Resets every index in prev_sensor_triggered to false except for the specified index.
 * 
 * Was used in the softlaunch (before loop iteration -- meaning the version w/ interactivity w/ buttons), effects were
 * purely driven by sensor inputs. This was used to just clear the array which said which sensor was last triggered (which
 * was checked to see what effect to run), and was called whenever a new sensor trigger was detected.
 *
 * TLDR; kinda deprecated, but might be used later if necessary -- need testing w/ sensors on real field (just reduces code dup.)
 * 
 *  -- Note: You can still see how it was used in the commented out code in loop rn (11/29/2024 @ 6pm)
 */
void resetTriggerMarkers(int exception) {
    for (int i = 0; i < prev_sensor_triggered.size(); i++) {
        if (i == exception) {
            prev_sensor_triggered[i] = true;
        } else {
            prev_sensor_triggered[i] = false;
        }
    }
}
#endif
