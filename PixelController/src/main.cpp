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
#include "effects.h"

#endif

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
void newEffectReset();
void generateFrame(ControllerRunner::ShowFrame showframe);

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
int* prevLeds1 = NULL;
// int* prevLeds2;
// int* prevLeds3;
// int* prevLeds4;
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
unsigned long end_millis;
unsigned long frame_runtime; // total time taken to generate the frame

FileManager *fm = NULL;

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
    loadImagesFromSD(imgs, fm, NUM_LEDS_X, NUM_LEDS_Y);
    std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;
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


    end_millis = millis();
    frame_runtime = end_millis - current_millis;
    delay((frame_runtime >= 33) ? 0 : (33 - frame_runtime));  // keep frames coming out as close to 30fps as possible
}


// MARK: Functions

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

            rippleEffect(leds, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter++, prevLeds1, 2);
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

            shiftLeds(leds, NUM_LEDS_X, NUM_LEDS_Y, RIGHT);  // might make the rainbow effect look v weird, not sure
            break;

        case pumpkinRipple:
            std::cout << "Generating pumpkinRipple frame..." << std::endl;
            if (curEffect != 3) {
                newEffectReset;
                curEffect = 3;
            }

            // draw ripple with pumpkin on top
            rippleEffect(leds, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter++, prevLeds1, 2);

            // load the images (check locations)                                                                 // v b/c shifting right
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (0 + count) % NUM_LEDS_X, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (25 + count) % NUM_LEDS_X, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (50 + count) % NUM_LEDS_X, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (75 + count) % NUM_LEDS_X, 0, true);

            shiftLeds(leds, NUM_LEDS_X, NUM_LEDS_Y, RIGHT);  // calls FastLED.show()
            rippleCounter++;         // increment again to account for the shift (if it looks weird just remove this)
            count++;
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

            shiftLeds(leds, NUM_LEDS_X, NUM_LEDS_Y, RIGHT);  // calls FastLED.show()
            break;

        case ghostRipple:
            std::cout << "Generating ghostRipple frame..." << std::endl;
            if (curEffect != 5) {
                newEffectReset;
                curEffect = 5;
            }

            rippleEffect(leds, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter++, prevLeds1, 2);

            // load the images (check locations)                                                               // v b/c shifting right
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (0 + count) % NUM_LEDS_X, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (25 + count) % NUM_LEDS_X, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (50 + count) % NUM_LEDS_X, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (75 + count) % NUM_LEDS_X, 0, true);

            shiftLeds(leds, NUM_LEDS_X, NUM_LEDS_Y, RIGHT);
            rippleCounter++;  // increment again to account for the shift (if it looks weird just remove this)
            count++;
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

            shiftLeds(leds, NUM_LEDS_X, NUM_LEDS_Y, RIGHT);  // calls FastLED.show()
            break;

        case pumpkinGhostRipple:
            std::cout << "Generating pumpkinGhostRipple frame..." << std::endl;
            if (curEffect != 7) {
                newEffectReset;
                curEffect = 7;
            }

            // draw ripple, then pumpkin and ghost
            rippleEffect(leds, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter++, prevLeds1, 2);

            // load the images (check locations)                                                                 // v b/c shifting right
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (0 + count) % NUM_LEDS_X, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (25 + count) % NUM_LEDS_X, 0, true);
            bufferToCRGBArray(pumpkinjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (50 + count) % NUM_LEDS_X, 0, true);
            bufferToCRGBArray(ghostjpg, 25, 24, loadedImageChannels, leds, NUM_LEDS_X, NUM_LEDS_Y, (75 + count) % NUM_LEDS_X, 0, true);

            shiftLeds(leds, NUM_LEDS_X, NUM_LEDS_Y, RIGHT);  // calls FastLED.show()
            rippleCounter++;         // increment again to account for the shift (if it looks weird just remove this)
            count++;
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

            shiftLeds(leds, NUM_LEDS_X, NUM_LEDS_Y, DOWN);  // calls FastLED.show()
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

            shiftLeds(leds, NUM_LEDS_X, NUM_LEDS_Y, RIGHT);  // calls FastLED.show()
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

            shiftLeds(leds, NUM_LEDS_X, NUM_LEDS_Y, RIGHT);  // calls FastLED.show()
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

            shiftLeds(leds, NUM_LEDS_X, NUM_LEDS_Y, DOWN);  // calls FastLED.show()
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
    // prevLeds1 = {0}; // commented out b/c not using this anymore (memory issues on esp32), see rippleEffect.cpp for changed implementation
    rippleCounter = 0;
    curEffect = -1;

    // might be used? no harm in resetting if not
    count = 0;
    goUp = true;
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
