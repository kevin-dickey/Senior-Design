
#include "utils.h"

unsigned char bufferPattern[3][3][3] = {
    {{0x00, 0x00, 0xFF}, {0x00, 0x00, 0xFF}, {0x00, 0xFF, 0x00}},
    {{0x00, 0x00, 0xFF}, {0x00, 0xFF, 0x00}, {0xFF, 0xFF, 0xFF}},
    {{0x00, 0xFF, 0x00}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}}};

unsigned char *bufferPtr = &bufferPattern[0][0][0];

/**
 * MARK: Distance calculation
 */
/**
 * Calculates the distance between two (x, y) points provided.
 */
uint16_t calculateDistance(uint16_t center_x, uint16_t center_y, uint16_t x, uint16_t y)
{
    // Calculate Euclidean distance from center point to point (x, y)
    int dx = x - center_x;
    int dy = y - center_y;
    return static_cast<uint16_t>(sqrt(dx * dx + dy * dy));
}

/**
 * Calculates the (x, y) position of a grid of LEDs.
 * This is required over just creating a 2D array as it depends whether the LEDs
 * are setup in a serpentine manner, and if they're setup ordered in a vertical manner or not.
 *
 * If something doesn't look right, try changing the value of kMatrixVertical above.
 * If that doesn't work, try changing kMatrixSerpentineLayout (not applicable for testbench, we know the value it needs to be).
 */
uint16_t XY(uint8_t x, uint8_t y)
{
    int i;

    if (kMatrixSerpentineLayout == false)
    {
        if (kMatrixVertical == false)
        {
            i = (y * kMatrixWidth) + x;
        }
        else
        {
            i = kMatrixHeight * (kMatrixWidth - (x + 1)) + y;
        }
    }

    if (kMatrixSerpentineLayout == true)
    {
        if (kMatrixVertical == false)
        {
            if (y & 0x01)
            {
                // Odd rows run backwards
                uint8_t reverseX = (kMatrixWidth - 1) - x;
                i = (y * kMatrixWidth) + reverseX;
            }
            else
            {
                // Even rows run forwards
                i = (y * kMatrixWidth) + x;
            }
        }
        else
        { // vertical positioning
            if (x & 0x01)
            {
                i = kMatrixHeight * (kMatrixWidth - (x + 1)) + y;
            }
            else
            {
                i = kMatrixHeight * (kMatrixWidth - x) - (y + 1);
            }
        }
    };

    return i;
}

void rearrangeForSerpentine(CRGB *originalArray, CRGB *rearrangedArray, int width, int height)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int originalIndex = y * width + x;
            int rearrangedIndex = XY(x, y);
            rearrangedArray[rearrangedIndex] = originalArray[originalIndex];
        }
    }
}

/**
 * @brief 
 * Rearranges rows of the original array into the rearranged array in a serpentine pattern.
 * 
 * Example: width = 20, height = 6, groupSize = 3
 * Then, num_groups would be 2.
 * The nth rows in each group are concatenated together in the rearranged array.
 * If reverse is true, then the rows in every other group starting with the second group are reversed.
 * 
 * In this example, Strip 1 has rows 0 and 3. Strip 2 has rows 1 and 4. Strip 3 has rows 2 and 5.
 * With reverse = true, rows 3, 4, 5 are reversed.
 * 
 * Expects rearrangedArray to be height / groupSize in length.
 * @param originalArray  Original array of pixels
 * @param rearrangedArray  Array of arrays to store rearranged rows (len = groupSize)
 * @param width  Total number of columns in the original array
 * @param height Total number of rows in the original array
 * @param groupSize Number of strands in a group (going a single direction)
 */
// Version 3
void rearrangeForGroupedSerpentine(CRGB *originalArray, CRGB **rearrangedArrays, int width, int height, int groupSize, bool reverse) {
    // Validate constraints
    if (height % groupSize != 0) {
        std::cerr << "Error: Total rows must be divisible by group size." << std::endl;
        return;
    }

    // Number of groups
    int numGroups = height / groupSize;

    for (int strand = 0; strand < groupSize; strand++) {
        for (int group = 0; group < numGroups; group++) {
            // Calculate the original row for this strand and group
            int originalRow = ((numGroups - 1 - group) * groupSize) + strand;

            // Determine if this row should be reversed (every other group)
            bool isRowReversed = reverse && ((originalRow / groupSize) % 2 == 0);

            // Iterate through pixels in the row
            for (int col = 0; col < width; col++) {
                // Calculate original index
                int originalIndex = originalRow * width + 
                    (isRowReversed ? (width - 1 - col) : col);
                
                // Calculate the index within the strand's array
                int strandIndex = group * width + col;

                // Copy pixel to the appropriate strand array
                rearrangedArrays[strand][strandIndex] = originalArray[originalIndex];
            }
        }
    }

    // Verify last row requirement
    int expectedLastRow = height - 1;
    int actualLastRowInRearranged = ((groupSize - 1) + (numGroups - 1) * groupSize);
    
    if (expectedLastRow != actualLastRowInRearranged) {
        std::cerr << "Error: Last row mismatch. Expected " << expectedLastRow 
                  << ", but got " << actualLastRowInRearranged << std::endl;
    }
}

void rearrangeForStrips(CRGB *originalArray, CRGB **rearrangedArrays, int width, int height)
{
    if (rearrangedArrays[0] == nullptr)
    {
        std::cout << "Error: The rearranged arrays must be initialized before calling this function." << std::endl;
        return;
    }

    // Split the original array into rows to display on each strip.
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int originalIndex = y * width + x;
            int rearrangedIndex = x;
            rearrangedArrays[y][rearrangedIndex] = originalArray[originalIndex];
        }
    }
}

/**
 * Makes sure the specified point is in bounds before calculating its (x, y) position.
 */
uint16_t XYsafe(uint8_t x, uint8_t y)
{
    if (x >= kMatrixWidth)
        return -1;
    if (y >= kMatrixHeight)
        return -1;
    return XY(x, y);
}

extern unsigned char *ghostjpg;
extern unsigned char *pumpkinjpg;
extern unsigned char *candyCanejpg;
extern unsigned char *christmasTreejpg;
extern unsigned char *snowflakejpg;
extern unsigned char *snowmanjpg;
extern int loadedImageHeight, loadedImageWidth, loadedImageChannels;

/**
 * Loads in images from the SD card, images should be a string of the filepath (e.g. "/djibouti.jpg")
 * TODO: This shouldn't be in the patternGeneration library. We should move this and other File-related things to a new library.
 */
void loadImagesFromSD(std::vector<std::string> images, FileManager *fm, int leds_x, int leds_y)
{
    for (const std::string &image : images)
    {
        std::cout << "📂 Loading " << image << " Image..." << std::endl;
        try
        {
            File jpgFile = fm->getJsonFile(image);
            std::cout << "✅ Opened " << image << " File!" << std::endl;

            size_t fileSize;
            unsigned char *fileBuf = ImageProcessing::convertFsFileToBuffer(&jpgFile, fileSize); // <--
            std::cout << "✅ Converted to FILE!" << std::endl;

#ifdef DEBUG_MODE
            std::cout << "  File Size: " << fileSize << std::endl;
            std::cout << "  First 10 bytes: " << fileBuf[0] << fileBuf[1] << fileBuf[2] << fileBuf[3] << fileBuf[4] << fileBuf[5] << fileBuf[6] << fileBuf[7] << fileBuf[8] << fileBuf[9] << std::endl;
#endif

            jpgFile.close();
            std::cout << "✅ Closed ghost from file manager!" << std::endl;

            std::cout << "⏳ Getting Image Dimensions..." << std::endl;
            // Get the dimensions of the image and load it
            ImageProcessing::get_image_dimensions_from_memory(fileBuf, fileSize, &loadedImageWidth, &loadedImageHeight, &loadedImageChannels);
            std::cout << "  Image Width: " << loadedImageWidth << std::dec << std::endl;
            std::cout << "  Image Height: " << loadedImageHeight << std::dec << std::endl;
            std::cout << "  Image Channels: " << loadedImageChannels << std::dec << std::endl;

            // get largest free heap block
            std::cout << "Largest free heap block: " << ESP.getMaxAllocHeap() << std::endl;

            unsigned char *imageFile = ImageProcessing::load_image_from_memory(fileBuf, fileSize, &loadedImageWidth, &loadedImageHeight, &loadedImageChannels);
            std::cout << "✅ Loaded Image!" << std::endl;
            // making it always resize for now

            std::cout << "↔️ Resizing " << image << " Image..." << std::endl;
            int newWidth = leds_y / 2; // A larger width will resize downto the correct width to maintain aspect ratio with preserve_ratio = true
            int newHeight = leds_y / 2;
            imageFile = ImageProcessing::resize_image(imageFile, loadedImageWidth, loadedImageHeight, loadedImageChannels, newWidth, newHeight, false);

// #ifdef DEBUG_MODE
            std::cout << "🖼️ Resized Image:" << std::endl;
            std::cout << "   Image Width: " << newWidth << std::endl;
            std::cout << "   Image Height: " << newHeight << std::endl;
            std::cout << "   Image Channels: " << loadedImageChannels << std::endl;
// #endif

            std::cout << "🕊️ Freeing File Buffer..." << std::endl;
            free(fileBuf);

// DEBUG: Print the loaded image data
#ifdef DEBUG_MODE
            std::cout << "🖼️ Displaying Image Data..." << std::endl;
            ImageProcessing::printImageHex(imageFile, newWidth, newHeight, loadedImageChannels);
#endif

            // Store the image
            std::cout << "💾 Storing image to the global pointer..." << std::endl;
            if (image == "/santahat.png")
            {
                ghostjpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            }
            else if (image == "/christmastree.jpg")
            {
                pumpkinjpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            }
            else if (image == "/candycane.png")
            {
                candyCanejpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            }
            else if (image == "/snowflake.png")
            {
                snowflakejpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            }
            else if (image == "/christmastree.jpg")
            {
                christmasTreejpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            }
            else if (image == "/snowman.png")
            {
                snowmanjpg = imageFile;
                std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
            }
            else
            {
                std::cerr << "Unrecognized file supplied (should be a jpg): '" << image << "' !" << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error loading image: " << e.what() << std::endl;
        }
    }
    std::cout << "😁 Done loading images!" << std::endl;
}

// Function to convert a 6-character hex string to CRGB
CRGB hexToCRGB(const char *hex)
{
    uint8_t r = strtol(std::string(hex, 2).c_str(), NULL, 16);
    uint8_t g = strtol(std::string(hex + 2, 2).c_str(), NULL, 16);
    uint8_t b = strtol(std::string(hex + 4, 2).c_str(), NULL, 16);
    return CRGB(r, g, b);
}

uint8_t bufferToCRGBArray(unsigned char *buffer, int imgWidth, int imgHeight, int imgChannels, CRGB *leds, int matrixWidth, int matrixHeight, int startX, int startY, bool wrap)
{
    if (startX < 0 || startY < 0)
    {
        return 1;
    }

    if (startX >= matrixWidth || startY >= matrixHeight)
    {
        return 1;
    }

    for (int y = 0; y < imgHeight; ++y)
    {
        bool rowFinished = false;

        for (int x = 0; x < imgWidth; ++x)
        {
            int bufferIndex = (y * imgWidth + x) * imgChannels;
            uint16_t matrixIndex = 0;
            if (wrap)
            {
                matrixIndex = ((startY + y) % matrixHeight) * matrixWidth + ((startX + x) % matrixWidth);
            }
            else
            {
                matrixIndex = (startY + y) * matrixWidth + (startX + x);
            }

            if (matrixIndex >= matrixWidth * matrixHeight)
            {
                rowFinished = true;
                break;
            }

            if (imgChannels == 3)
            { // RGB
                leds[matrixIndex] = CRGB(buffer[bufferIndex], buffer[bufferIndex + 1], buffer[bufferIndex + 2]);
            }
            else if (imgChannels == 4)
            { // RGBA
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

// Print a CRGB pixel as a hex string
void printCRGBPixel(CRGB pixel)
{
    std::cout << "0x"
              << std::hex << std::setw(2) << std::setfill('0') << (int)pixel.r
              << std::setw(2) << std::setfill('0') << (int)pixel.g
              << std::setw(2) << std::setfill('0') << (int)pixel.b
              << std::dec;
}

void print_crgb_frame(CRGB *leds, int matrixWidth, int matrixHeight)
{
    uint8_t channels = 3;

    for (int y = 0; y < matrixHeight; ++y)
    {
        for (int x = 0; x < matrixWidth; ++x)
        {
            printCRGBPixel(leds[y * matrixWidth + x]);
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}