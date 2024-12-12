
#include "utils.h"

/**
 * MARK: Distance calculation
 */
/**
 * Calculates the distance between two (x, y) points provided.
 */
uint8_t calculateDistance(uint8_t center_x, uint8_t center_y, uint8_t x, uint8_t y)
{
    // Calculate Euclidean distance from center point to point (x, y)
    int dx = x - center_x;
    int dy = y - center_y;
    return static_cast<uint8_t>(sqrt(dx * dx + dy * dy));
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

void rearrangeForGroupedSerpentine(CRGB *originalArray, CRGB *rearrangedArray, int width, int height, int groupSize)
{
    int numGroups = height / groupSize;

    for (int strand = 0; strand < height; ++strand)
    {
        // Number of groups in the matrix, where each group contains groupSize rows (strands)
        int groupIndex = strand / groupSize;

        // Flip odd
        bool isFlipped = (groupIndex % 2 != 0);

        // Calculate the rearranged strand position
        int rearrangedStrand;

            if (isFlipped)
            {
                // Starting strand of the current group
                int groupStartStrand = groupIndex * groupSize;

                // Ending strand of the current group
                int groupEndStrand = groupStartStrand + groupSize - 1;

                // Reverse the strand
                rearrangedStrand = groupEndStrand - (strand % groupSize);
            }
            else
            {
                rearrangedStrand = strand;
            }

        // Copy all elements in this strand (row)
        for (int col = 0; col < width; ++col)
        {
            int originalIndex = strand * width + col;
            int rearrangedIndex = rearrangedStrand * width + col;

            rearrangedArray[rearrangedIndex] = originalArray[originalIndex];
        }
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
        try
        {
            File jpgFile = fm->getJsonFile(image);
            std::cout << "✅ Opened " << image << " File!" << std::endl;

            size_t fileSize;
            unsigned char *fileBuf = ImageProcessing::convertFsFileToBuffer(&jpgFile, fileSize); // <--
            std::cout << "✅ Converted to FILE!" << std::endl;

#if DEBUG_MODE
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

            unsigned char *imageFile = ImageProcessing::load_image_from_memory(fileBuf, fileSize, &loadedImageWidth, &loadedImageHeight, &loadedImageChannels);
            std::cout << "✅ Loaded Image!" << std::endl;
            // making it always resize for now

            // hard coding the resize to be 24x25, given the field is 24x100. for future fields just always resize img to what user wants/how many times they want it repeated
            // resizing only ghost and pumpkin (or unrecognized file), might just want to recreate them to be right dimensions tbh
            // if (image == "/8bitghost.jpg" || "/8bitpumpkin.jpg" || !("/candycane.jpg" || "/snowflake.jpg" || "/christmastree.jpg" || "/snowman.jpg"))
            // {
                std::cout << "↔️ Resizing " << image << " Image..." << std::endl;
                int newWidth = leds_x / 4;
                int newHeight = leds_y;
                imageFile = ImageProcessing::resize_image(imageFile, loadedImageWidth, loadedImageHeight, loadedImageChannels, newWidth, newHeight, true);

#if DEBUG_MODE
                std::cout << "🖼️ Resized Image:" << std::endl;
                std::cout << "   Image Width: " << newWidth << std::endl;
                std::cout << "   Image Height: " << newHeight << std::endl;
                std::cout << "   Image Channels: " << loadedImageChannels << std::endl;
#endif

                std::cout << "🕊️ Freeing File Buffer..." << std::endl;
                free(fileBuf);

// DEBUG: Print the loaded image data
#if DEBUG_MODE
                std::cout << "🖼️ Displaying Image Data..." << std::endl;
                ImageProcessing::printImageHex(imageFile, 24, 25, loadedImageChannels);
#endif

                // Store the image
                std::cout << "💾 Storing image to the global pointer..." << std::endl;
                if (image == "/8bitghost.png")
                {
                    ghostjpg = imageFile;
                    std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
                }
                else if (image == "/8bitpumpkin.png")
                {
                    pumpkinjpg = imageFile;
                    std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
                }
                else if (image == "/blue.png")
                {
                    candyCanejpg = imageFile;
                    std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
                }
                else if (image == "/snowflake.png")
                {
                    snowflakejpg = imageFile;
                    std::cout << "✅ Successfully stored '" << image << "' !" << std::endl;
                }
                else if (image == "/christmastree.png")
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
        // int startX = (matrixWidth - imgWidth) / 2;
        // int startY = (matrixHeight - imgHeight) / 2;
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
            for (int x = 0; x < imgWidth; ++x)
            {
                int bufferIndex = (y * imgWidth + x) * imgChannels;
                uint8_t matrixIndex = 0;
                if (wrap)
                {
                    matrixIndex = ((startY + y) % matrixHeight) * matrixWidth + ((startX + x) % matrixWidth);
                }
                else
                {
                    matrixIndex = (startY + y) * matrixWidth + (startX + x);
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

    void fillRemainingPixels(CRGB * leds, int matrixWidth, int matrixHeight, CRGB backgroundColor)
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
