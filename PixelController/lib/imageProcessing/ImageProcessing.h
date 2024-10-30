//
// Created by Nick Vazquez on 10/15/24.
//

#ifndef PIXELCONTROLLER_IMAGEPROCESSING_H
#define PIXELCONTROLLER_IMAGEPROCESSING_H

#include <FS.h>

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG

class ImageProcessing {

public:
    typedef struct ImageData {
        unsigned char *data;
        int width;
        int height;
        int channels;
    } ImageData_t;

    static unsigned char *convertFsFileToBuffer(fs::File *fsFile, size_t &fileSize);

    static int get_image_dimensions(const char *filename, int *width, int *height, int *channels);

    static int get_image_dimensions_from_memory(unsigned char* buffer, size_t &len, int *outWidth, int *outHeight, int *outChannels);
    
    static unsigned char *load_image(const char *filename, int *width, int *height, int *channels);

    static unsigned char *load_image_from_memory(unsigned char *buffer, size_t &len, int *width, int *height, int *channels);

    static void free_image(unsigned char *data);

    static unsigned char *
    resize_image(const unsigned char *image, int width, int height, int channels,
                 int &new_width, int &new_height, bool preserve_ratio = true);
};


#endif //PIXELCONTROLLER_IMAGEPROCESSING_H
