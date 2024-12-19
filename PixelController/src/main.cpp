#include <iostream>
#include <chrono>
#include "json.hpp"
using json = nlohmann::json;

#define STRIP_1_PIN 32
#define STRIP_2_PIN 33
#define STRIP_3_PIN 25
#define STRIP_4_PIN 26
#define STRIP_5_PIN 27
#define STRIP_6_PIN 14
#define STRIP_7_PIN 12

#define FRAMES_PER_SECOND 30
#define MAX_BRIGHTNESS 127 // maximum for FastLED is 255, (don't go higher than like 8 if you don't have a PSU attached)
#define SKIP_SHOW_INITIALIZATION 0

#if USE_EMULATOR
#define PROJECT_DIR SOURCE_ROOT

// TODO: Gotta do this for anything in lib while using CMake. PlatformIO automatically adds the include path for lib
//  so that we can include the headers directly (<Configuration.h>). This is a workaround for CMake.
#include "configuration/FileManager.h"
#include "configuration/Configuration.h"
#include "configuration/ControllerRunner.h"
#include "imageProcessing/ImageProcessing.h"
#include "patternGeneration/rippleEffect.h"
#include "patternGeneration/utils.h"
#include "config.h"

#else
#define COLOR_ORDER RGB
#define CHIPSET WS2812B

#include <FastLED.h>
#include <FileManager.h>
#include <Configuration.h>
#include <ControllerRunner.h>
#include <ImageProcessing.h>
#include <effects.h>
#include <rippleEffect.h>
#include <Sensor.h>
#include <utils.h>

#include "config.h"

#endif

FileManager *fm = NULL;
Show_t show;
SensorManager *sensorManager;
ControllerRunner *runner;

std::string showName = "show.json";
int num_leds_x = 100;
int num_leds_y = 24;
int num_groups = 4;      // number of groups of strips. This should be the height of all pixels divided by the number of strands.
int leds_per_group;      // number of leds per group of strips
int effect_spacing = 20; // TODO: Give this a better name - it's the spacing between images in our moving effects.
bool reverseGroups = true;

// Array of the original frame data and the transposed strip data from the foreground_frame.
CRGB *foreground_frame;
CRGB **strip_data; // len(num_leds_x)

// misc. stuff (effect variables, sensor stuff, running-time)
int hue;
int curEffect = -1;
int groupSizeStrands = num_leds_y / num_groups;
int num_leds = num_leds_x * num_leds_y;
int count;
bool goUp;
bool loaded = false;
char set_sensors;

// Timekeeping
std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 1000000000>>> epoch;
unsigned long showStart = 0;
unsigned long current_millis;
unsigned long end_millis;
unsigned long frame_runtime; // total time taken to generate the frame

unsigned long getMillis()
{
#if USE_EMULATOR
    auto now = std::chrono::system_clock::now();
    auto mseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - epoch).count();
    return mseconds;
#else
    return millis();
#endif
}

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
            epoch = std::chrono::system_clock::from_time_t(0);
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
            epoch = std::chrono::system_clock::from_time_t(0);
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

SET_LOOP_TASK_STACK_SIZE(32 * 1024); // 16KB

void newEffectReset();
void generateFrame(ControllerRunner::ShowFrame showframe);

#pragma region Embedded Variables
int LEDS_SIZE_ARR[2] = {num_leds_x, num_leds_y};
uint8_t kMatrixWidth;
uint8_t kMatrixHeight;

// ripple effect stuff
int *prevLeds1 = NULL;
int rippleCounter;

// sensor & show stuff
Sensor *sensor0, *sensor1, *sensor2, *sensor3;
std::vector<Sensor *> a_sensors;
std::vector<bool> prev_sensor_triggered;

unsigned char *ghostjpg;
unsigned char *pumpkinjpg;
unsigned char *candyCanejpg;
unsigned char *christmasTreejpg;
unsigned char *snowflakejpg;
unsigned char *snowmanjpg;
unsigned char *isuLogo;
#pragma endregion // Variables

int loadedImageHeight, loadedImageWidth, loadedImageChannels;
unsigned char *loadedImage;

void clearStrips(CRGB **strip_data, int num_leds_x, int num_leds_y)
{
    FastLED.clear();
}

// MARK: Setup
void setup()
{
    Serial.begin(115200); // for setting up stuff to print to serial monitor
    // delay(3000);                             // delay for 3 seconds to give time to open the serial monitor
    std::cout << "Starting..." << std::endl; // print to the serial monitor that the program is starting

#if SKIP_SHOW_INITIALIZATION
#else

    // SD Card Setup
    fm = new FileManager();
    if (!fm->MountFileSystem())
    {
        std::cout << "Failed to mount file system. Halting..." << std::endl;
        while (true)
            ;
    }

#pragma region Show Initialization
    try
    {
        File showFile = fm->getJsonFile(showName);
        show = loadShow(showFile);
        std::cout << "✅ Loaded Show!" << std::endl;
        // std::cout << "➡️ Show Name: " << show.name << std::endl;
        runner = new ControllerRunner(show, millis(), epoch);
        showFile.close();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading show: " << e.what() << std::endl;
    }

    std::cout << "Deserializing layout:" << std::endl;
    if (show.layouts[0]->isGridLayout())
    {                                             // might need to be a try catch instead (isGridLayout not defined for other types, but other types also not rlly defined afaict)
        num_leds_x = show.layouts[0]->getWidth(); // this returns size of frontend, SHOULD be 100
        num_leds_y = show.layouts[0]->getHeight();
        num_leds = num_leds_x * num_leds_y;
        leds_per_group = num_leds_x * num_groups;

        kMatrixHeight = num_leds_y;
        kMatrixWidth = num_leds_x;

        std::cout << "Set width/height to: " << num_leds_x << "x" << num_leds_y << std::endl;

        foreground_frame = new CRGB[num_leds];
        strip_data = new CRGB *[groupSizeStrands]; // Number of strips
        for (int i = 0; i < groupSizeStrands; i++)
        {
            // Allocate space for each width of the strip
            strip_data[i] = new CRGB[leds_per_group];
        }

        // waiting on confirm if you want to double the computational intensity for ripple effect in lieu of saving on storage
        prevLeds1 = new int[num_leds];
    }
    else
    {
        // custom layout, not gonna bother with this rn but you'll have to set the same variables in some way (there aren't height and width params passed)
    }
#pragma endregion // Show Initialization
#endif

    showStart = getMillis();

#pragma region FastLED Initialization
    // TODO: Set length from number of rows
    FastLED.addLeds<CHIPSET, STRIP_2_PIN, COLOR_ORDER>(strip_data[5], leds_per_group).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_3_PIN, COLOR_ORDER>(strip_data[4], leds_per_group).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_4_PIN, COLOR_ORDER>(strip_data[3], leds_per_group).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_5_PIN, COLOR_ORDER>(strip_data[2], leds_per_group).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_6_PIN, COLOR_ORDER>(strip_data[1], leds_per_group).setCorrection(TypicalSMD5050);
    FastLED.addLeds<CHIPSET, STRIP_7_PIN, COLOR_ORDER>(strip_data[0], leds_per_group).setCorrection(TypicalSMD5050);

    FastLED.setBrightness(MAX_BRIGHTNESS); // set the max brightness for the LEDs
    pinMode(LED_BUILTIN, OUTPUT);          // setup the built-in LED for the esp32

    fill_solid(foreground_frame, num_leds, CRGB::Black);
    clearStrips(strip_data, num_leds_x, num_leds_y);
    FastLED.show();

    Serial.println("Initialized FastLED...");
    std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;
#pragma endregion // FastLED Initialization

#pragma region Sensor Initialization
    // TODO: Read and configure sensors from the show file
    sensorManager = new SensorManager();
    // sensor1 = new Sensor(1, 35, 1000, S_BINARY, sensor_pos, sensorEffect);  // nw
    // Effect *sensorEffect = new Effect(1, rainbow, "BasicEffect", sensor_pos, size, 0.0, 500.0, std::move(translation));
    // sensors = std::vector<Sensor *>{sensor1};
    // sensorManager->setSensors(sensors);
    Serial.println("Initialized Sensors...");
#pragma endregion // Sensor Initialization

    // Get the largest free block of heap memory
    std::cout << "Largest free heap block: " << ESP.getMaxAllocHeap() << std::endl;

    // for (effect which uses an image : show) {
    //     append necessary filepaths to a std::vector<std::string> or whatever datatype you want
    // }
    // TODO: Dynamic loading of images based on effect. Save memory by loading only the images needed for the current effect running.
    std::vector<std::string> imgs = {
        "/santahat.png",        // ghost
        "/8bitpumpkin.png",     // Pumpkin
        "/candycane.png",       // Candy Cane
        "/snowman.png",         // Snowman
        "/snowflake.png",       // Snowflake
    };

    loadImagesFromSD(imgs, fm, show.layouts[0]->getWidth(), show.layouts[0]->getHeight());
    std::cout << "😁 Done loading images!" << std::endl;
    std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;

    hue = 30;
    count = 0;
    rippleCounter = 0;
    goUp = true;
    srand(static_cast<unsigned int>(time(0)));

    delay(5000); // delay for 5 seconds to give time to open the serial monitor
}

/**
 * MARK: Looping
 */
void loop()
{
    // gets the current state of every sensor, the state automatically resets after it's viewed
    auto sensor_states = sensorManager->getSensorStates(true);
    current_millis = millis();

    // sets the active sensors so that the runner is constanly checking sensor state and determining what to display
    // get showFrame
    auto showFrame = runner->getNextShowFrame(sensor_states);

    // std::cout << "Show Frame: [" << showFrame.effect->name << ", " << showFrame.frame << "]" << std::endl;

    while (showFrame.effect->name == "no effect" && showFrame.frame == -1)
    {
        std::cout << "No effect found. Resetting runner..." << std::endl;
        std::cout << "💩 Available Heap: " << ESP.getFreeHeap() << std::endl;

        // no effect found because we're over the shows duration. reset show
        File showFile = fm->getJsonFile(showName);
        show = loadShow(showFile);
        showFile.close();

        newEffectReset();

        showStart = getMillis();
        epoch = std::chrono::system_clock::from_time_t(0);
        delete runner;
        runner = new ControllerRunner(show, showStart, epoch);
        showFrame = runner->getNextShowFrame(sensor_states);

        std::cout << "New frame after reset: [" << showFrame.effect->name << ", " << showFrame.frame << "]" << std::endl;
    }

    // TODO: divide frame up to send to picos
    try
    {
        generateFrame(showFrame);
        clearStrips(strip_data, num_leds_x, num_leds_y);
        rearrangeForGroupedSerpentine(
            foreground_frame, strip_data,
            num_leds_x, num_leds_y,
            groupSizeStrands, reverseGroups);
        FastLED.show();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error generating frame: " << showFrame.effect->name << ", " << showFrame.frame << "]" << std::endl;
        std::cerr << e.what() << '\n';
    }

    end_millis = millis();
    frame_runtime = end_millis - current_millis;
    delay((frame_runtime >= (1000 / FRAMES_PER_SECOND)) ? 0 : ((1000 / FRAMES_PER_SECOND) - frame_runtime));
}

/**
 * Builds frame onto the leds array (CRGB leds[]) based on the provided showframe.
 *
 * Limited -- only generates frames based on the specified EffectTypes in the backend (Effect.h),
 *         -- which is based on the frontend EffectTypes, but the backend needs to be manually updated to
 *         -- whatever EffectTypes the frontend has.
 */
void generateFrame(ControllerRunner::ShowFrame showframe)
{
    const uint8_t frames_per_shift = 5;
    const uint8_t max_shifts = 60;

    uint8_t start_x = showframe.effect->origin.x;
    uint8_t start_y = showframe.effect->origin.y;
    uint8_t size_x = showframe.effect->size.x;
    uint8_t size_y = showframe.effect->size.y;

    if (curEffect != showframe.effect->effectType) // new effect
    {
        std::cout << "Starting effect -> (X: " << static_cast<int>(start_x) << ", Y: " << static_cast<int>(start_y) << ") "
                  << "Size: (X: " << static_cast<int>(size_x) << ", Y: " << static_cast<int>(size_y) << ")" << std::endl;
    }

    // shift directions might be incorrect, need to test on field!
    // rainbow + shifting effects might look a little goofy, but shouldn't be completely broken i don't think
    // ripple + shifting effects almost surely broken af
    switch (showframe.effect->effectType)
    { // set-up the leds[] with the frame based on desired effect
    case rainbow:
        if (curEffect != rainbow)
        {
            newEffectReset();
            curEffect = rainbow;
        }

        drawRainbow(foreground_frame, current_millis);
        break;

    case ripple:
        if (curEffect != ripple)
        {
            newEffectReset();
            curEffect = ripple;
        }

        // Error if origin is null
        if (&showframe.effect->origin == NULL)
        {
            std::cerr << "Error: Ripple effect requires an origin point." << std::endl;
            return;
        }

        rippleEffect(foreground_frame, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter++, prevLeds1, 2);
        break;

    case pumpkinRainbow:
        std::cout << "Generating pumpkinRainbow frame..." << std::endl;
        if (curEffect != pumpkinRainbow)
        {
            newEffectReset();
            curEffect = pumpkinRainbow;
        }

        // draw rainbow with pumpkin on top
        drawRainbow(foreground_frame, current_millis);

        bufferToCRGBArray(pumpkinjpg,
                          size_x, size_y, 4,
                          foreground_frame,
                          num_leds_x, num_leds_y,
                          start_x, start_y,
                          true);

        if (showframe.frame % frames_per_shift == 0)
        {
            shiftLeds(foreground_frame, num_leds_x, num_leds_y, RIGHT);
        }
        break;

    case pumpkinRipple:
        if (curEffect != pumpkinRipple)
        {
            newEffectReset();
            curEffect = pumpkinRipple;
        }

        // draw ripple with pumpkin on top
        rippleEffect(foreground_frame, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter++, prevLeds1, 2);

        if (!loaded)
        {
            bufferToCRGBArray(pumpkinjpg,
                              size_x, size_y, 4,
                              foreground_frame,
                              num_leds_x, num_leds_y,
                              start_x, start_y,
                              true);
            loaded = true;
        }

        if (showframe.frame % frames_per_shift == 0)
        {
            shiftLeds(foreground_frame, num_leds_x, num_leds_y, RIGHT);
            rippleCounter++; // increment again to account for the shift (if it looks weird just remove this)
            count++;
        }
        break;

    case ghost:
        if (curEffect != ghost)
        {
            newEffectReset();
            curEffect = ghost;
        }

        // draw ghost
        bufferToCRGBArray(
            ghostjpg,
            size_x, size_y, 4,
            foreground_frame,
            num_leds_x, num_leds_y,
            start_x, start_y,
            true);
        break;

    case ghostRainbow:
        if (curEffect != ghostRainbow)
        {
            newEffectReset();
            curEffect = ghostRainbow;
        }

        // draw rainbow with ghost on top
        drawRainbow(foreground_frame, current_millis);

        bufferToCRGBArray(ghostjpg,
                          size_x, size_y, 4,
                          foreground_frame,
                          num_leds_x, num_leds_y,
                          start_x, start_y,
                          true);

        if (showframe.frame % frames_per_shift == 0)
        {
            shiftLeds(foreground_frame, num_leds_x, num_leds_y, RIGHT);
        }
        break;

    case ghostRipple:
    {
        if (curEffect != ghostRipple)
        {
            newEffectReset();
            curEffect = ghostRipple;
        }

        rippleEffect(foreground_frame, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter++, prevLeds1, 2);

        for (int x = start_x; x < num_leds_x; x += effect_spacing) // Repeat the image
        {
            bufferToCRGBArray(ghostjpg,
                              size_x, size_y, 4,
                              foreground_frame,
                              num_leds_x, num_leds_y,
                              x, start_y,
                              true);
        }

        shiftLeds(foreground_frame, num_leds_x, num_leds_y, RIGHT);
        rippleCounter++; // increment again to account for the shift (if it looks weird just remove this)
        count++;
        break;
    }
    case pumpkinGhostRainbow:
        if (curEffect != pumpkinGhostRainbow)
        {
            newEffectReset();
            curEffect = pumpkinGhostRainbow;

            if (start_x + size_x > num_leds_x || start_y + size_y > num_leds_y)
            {
                std::cerr << "Warning: Effect is too large for the LED matrix. Skipping..." << std::endl;
                if (start_x + effect_spacing + size_x > num_leds_x)
                    std::cerr << "Overflowing X: " << start_x + size_x << " > " << num_leds_x << std::endl;
                if (start_y + size_y > num_leds_y)
                    std::cerr << "Overflowing Y: " << start_y + size_y << " > " << num_leds_y << std::endl;
                std::cerr << "To fix this, reduce the size of the effect: " << showframe.effect->name << std::endl;
                return;
            }
        }

        // draw rainbow, then pumpkin and ghost
        drawRainbow(foreground_frame, current_millis);

        // need to set locations
        bufferToCRGBArray(pumpkinjpg,
                          size_x, size_y, 4,
                          foreground_frame,
                          num_leds_x, num_leds_y,
                          start_x, start_y, true);

        bufferToCRGBArray(ghostjpg,
                          size_x, size_y, 4,
                          foreground_frame,
                          num_leds_x, num_leds_y,
                          start_x + effect_spacing, start_y, false);

        // shiftLeds(foreground_frame, num_leds_x, num_leds_y, RIGHT);
        break;

    case pumpkinGhostRipple:
        if (curEffect != pumpkinGhostRipple)
        {
            newEffectReset;
            curEffect = pumpkinGhostRipple;

            if (start_x + size_x > num_leds_x || start_y + size_y > num_leds_y)
            {
                std::cerr << "Warning: Effect is too large for the LED matrix. Skipping..." << std::endl;
                if (start_x + effect_spacing + size_x > num_leds_x)
                    std::cerr << "Overflowing X: " << start_x + size_x << " > " << num_leds_x << std::endl;
                if (start_y + size_y > num_leds_y)
                    std::cerr << "Overflowing Y: " << start_y + size_y << " > " << num_leds_y << std::endl;
                std::cerr << "To fix this, reduce the size of the effect: " << showframe.effect->name << std::endl;
                return;
            }
        }

        rippleEffect(foreground_frame, LEDS_SIZE_ARR, 0, 255, 221, showframe.effect->origin.x, showframe.effect->origin.y, rippleCounter++, prevLeds1, 2);

        bufferToCRGBArray(
            pumpkinjpg,
            size_x, size_y, 4,
            foreground_frame,
            num_leds_x, num_leds_y,
            start_x, start_y,
            true);

        bufferToCRGBArray(
            ghostjpg,
            size_x, size_y, 4,
            foreground_frame,
            num_leds_x, num_leds_y,
            start_x + effect_spacing, start_y,
            true);

        if (showframe.frame % frames_per_shift == 0)
        {
            shiftLeds(foreground_frame, num_leds_x, num_leds_y, RIGHT);
            rippleCounter++; // increment again to account for the shift (if it looks weird just remove this)
            count++;
        }
        break;

    case snowflake:
        if (curEffect != snowflake)
        {
            newEffectReset();
            curEffect = snowflake;
        }

        if (!loaded)
        {
            bufferToCRGBArray(snowflakejpg,
                              size_x, size_y, 4,
                              foreground_frame,
                              num_leds_x, num_leds_y,
                              start_x, start_y,
                              true);
            loaded = true;
        }

        shiftLeds(foreground_frame, num_leds_x, num_leds_y, DOWN);
        break;

    case snowman:
    {
        if (curEffect != snowman)
        {
            newEffectReset();
            curEffect = snowman;
        }

        if (!loaded)
        {
            for (int x = start_x; (x + size_x) < num_leds_x; x += effect_spacing)
            {
                bufferToCRGBArray(snowmanjpg,
                                  size_x, size_y, 4,
                                  foreground_frame,
                                  num_leds_x, num_leds_y,
                                  x, start_y,
                                  true);
            }
            loaded = true;
        }

        // Only shift the candy cane if the frame is divisible by frames_per_shift
        if (showframe.frame % frames_per_shift == 0)
        {
            shiftLeds(foreground_frame, num_leds_x, num_leds_y, RIGHT);
        }

        break;
    }

    case pumpkin:
        if (curEffect != pumpkin)
        {
            newEffectReset();
            curEffect = pumpkin;

            std::cout << "Setting up pumpkin effect..." << std::endl;
        }

        if (!loaded)
        {
            for (int x = start_x; (x + size_x) < num_leds_x; x += effect_spacing) // Repeat the image
            {
                bufferToCRGBArray(pumpkinjpg,
                                  num_leds_y / 2, num_leds_y / 2, 4,
                                  foreground_frame,
                                  num_leds_x, num_leds_y,
                                  x, start_y,
                                  true);
            }
            loaded = true;
        }

        // Only shift the candy cane if the frame is divisible by 5
        if (showframe.frame % frames_per_shift == 0)
        {
            shiftLeds(foreground_frame, num_leds_x, num_leds_y, RIGHT);
        }
        break;

    case christmasTree:
        if (curEffect != christmasTree)
        {
            newEffectReset();
            curEffect = christmasTree;
        }

        if (!loaded)
        {
            // load in christmas tree & pattern onto leds
            bufferToCRGBArray(christmasTreejpg,
                              size_x, size_y, 4,
                              foreground_frame,
                              num_leds_x, num_leds_y,
                              start_x, start_y,
                              true);

            loaded = true;
        }

        shiftLeds(foreground_frame, num_leds_x, num_leds_y, RIGHT);
        break;

    case candyCane:
    {
        if (curEffect != candyCane)
        {
            newEffectReset();
            curEffect = candyCane;

            std::cout << "Setting up candyCane effect..." << std::endl;
        }

        if (!loaded)
        {
            for (int x = start_x; x + size_x < num_leds_x; x += effect_spacing) // Repeat the image
            {
                bufferToCRGBArray(candyCanejpg,
                                  size_x, size_y, 4,
                                  foreground_frame,
                                  num_leds_x, num_leds_y,
                                  x, start_y,
                                  true);
            }
            loaded = true;
        }

        // Only shift the candy cane if the frame is divisible by 5
        if (showframe.frame % frames_per_shift == 0)
        {
            shiftLeds(foreground_frame, num_leds_x, num_leds_y, DOWN);
        }

        break;
    }
    case rainbowWalker:
        if (curEffect != rainbowWalker)
        {
            newEffectReset();
            curEffect = rainbowWalker;
            hue = 30;
        }

        // draw rainbow with walker on top on each strip
        for (int i = 0; i < groupSizeStrands; i++)
        {
            fill_rainbow(strip_data[i], num_leds, (hue + 6 * i) % 256, 8);
        }

        hue += 2;
        hue %= 256;
        break;

    case ISU:
        if (curEffect != ISU)
        {
            newEffectReset();
            curEffect = ISU;
        }

        if (!loaded)
        {
            int imgWidth, imgHeight, imgChannels, newWidth, newHeight;
            size_t imgFilesize;

            File jpgFile = fm->getJsonFile("/trice_logo.png");
            unsigned char *fileBuf = ImageProcessing::convertFsFileToBuffer(&jpgFile, imgFilesize);
            jpgFile.close();
            ImageProcessing::get_image_dimensions_from_memory(fileBuf, imgFilesize, &imgWidth, &imgHeight, &imgChannels);
            isuLogo = ImageProcessing::load_image_from_memory(fileBuf, imgFilesize, &imgWidth, &imgHeight, &imgChannels);
            free(fileBuf);

            std::cout << "Loaded ISU logo: " << imgWidth << "x" << imgHeight << "x" << imgChannels << std::endl;

            newWidth = num_leds_x;
            newHeight = num_leds_y;

            isuLogo = ImageProcessing::resize_image(isuLogo, imgWidth, imgHeight, imgChannels, newWidth, newHeight, true);

            std::cout << "Width changed from " << imgWidth << " to " << newWidth << std::endl;

            uint8_t imgSize = 24;
            uint16_t mid_x = (num_leds_x / 2) - (newWidth / 2);
            uint16_t mid_y = (num_leds_y / 2) - (newHeight / 2);

                        fill_solid(foreground_frame, num_leds, CRGB::DarkRed);
            // draw ISU logo
            bufferToCRGBArray(
                isuLogo,
                newWidth, newHeight, 4,
                foreground_frame,
                num_leds_x, num_leds_y,
                mid_x, mid_y,
                false);

            loaded = true;
        }

        break;

    default:
        newEffectReset();
        std::cout << "  !Error! Effect not found/recognized (likely need to update Effect.h to match the effects on frontend)." << std::endl;
        std::cout << "Effect: " << showframe.effect->name << std::endl;
        return;
    }
}

// Resets some variables and such for the next effect to run properly
void newEffectReset()
{
    if (curEffect == ISU)
    {
        if (isuLogo != NULL)
            free(isuLogo);
        else
            std::cerr << "Error freeing ISU logo -> NULL" << std::endl;
    }

    loaded = false;
    // prevLeds1 = {0}; // commented out b/c not using this anymore (memory issues on esp32), see rippleEffect.cpp for changed implementation
    rippleCounter = 0;
    curEffect = -1;
    fill_solid(foreground_frame, num_leds, CRGB::Black);

    // might be used? no harm in resetting if not
    count = 0;
    goUp = true;
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
void resetTriggerMarkers(int exception)
{
    for (int i = 0; i < prev_sensor_triggered.size(); i++)
    {
        if (i == exception)
        {
            prev_sensor_triggered[i] = true;
        }
        else
        {
            prev_sensor_triggered[i] = false;
        }
    }
}
#endif
