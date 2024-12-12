//
// Created by Nick Vazquez on 10/15/24.
//

#ifndef PIXELCONTROLLER_IMAGEPROCESSING_H
#define PIXELCONTROLLER_IMAGEPROCESSING_H

#if USE_EMULATOR
#else
#include <FS.h>
#endif

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG

class ImageProcessing
{

public:
    typedef struct ImageData
    {
        unsigned char *data;
        int width;
        int height;
        int channels;
    } ImageData_t;

#if USE_EMULATOR
#else
    static unsigned char *convertFsFileToBuffer(fs::File *fsFile, size_t &fileSize);
#endif

    static int get_image_dimensions(const char *filename, int *width, int *height, int *channels);

    static int get_image_dimensions_from_memory(unsigned char *buffer, size_t &len, int *outWidth, int *outHeight, int *outChannels);

    static unsigned char *load_image(const char *filename, int *width, int *height, int *channels);

    static unsigned char *load_image_from_memory(unsigned char *buffer, size_t &len, int *width, int *height, int *channels);

    static void free_image(unsigned char *data);

    static unsigned char *
    resize_image(const unsigned char *image, int width, int height, int channels,
                 int &new_width, int &new_height, bool preserve_ratio = true);

    static void printImageHex(unsigned char *imageData,
                              uint8_t width,
                              uint8_t height,
                              uint8_t channels,
                              uint8_t bytesPerRow = 16,
                              uint8_t byte_separation = 1);
};

#endif // PIXELCONTROLLER_IMAGEPROCESSING_H
