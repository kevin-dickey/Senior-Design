//
// Created by Nick Vazquez on 10/15/24.
//

#ifndef PIXELCONTROLLER_IMAGEPROCESSING_H
#define PIXELCONTROLLER_IMAGEPROCESSING_H




#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG

class ImageProcessing {

public:
    static int get_image_dimensions(const char *filename, int *width, int *height, int *channels);
    static unsigned char *load_image(const char *filename, int *width, int *height, int *channels);
    static void free_image(unsigned char *data);
};


#endif //PIXELCONTROLLER_IMAGEPROCESSING_H
