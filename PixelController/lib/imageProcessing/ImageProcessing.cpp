//
// Created by Nick Vazquez on 10/15/24.
//

#include "ImageProcessing.h"

// Throw an error if STB_IMAGE_IMPLEMENTATION is already defined.
// This is to prevent multiple definitions of the same function.
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#else
#error "STB_IMAGE_IMPLEMENTATION is already defined!"
#endif

#ifndef STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"
#else
#error "STB_IMAGE_RESIZE_IMPLEMENTATION is already defined!"
#endif


int ImageProcessing::get_image_dimensions(const char *filename, int *width, int *height, int *channels) {
    return stbi_info(filename, width, height, channels);
}

unsigned char *ImageProcessing::load_image(const char *filename, int *width, int *height, int *channels) {
    return stbi_load(filename, width, height, channels, 0);
}

void ImageProcessing::free_image(unsigned char *data) {
    stbi_image_free(data);
}

unsigned char *ImageProcessing::resize_image(const unsigned char *image, int width, int height, int channels, int new_width, int new_height) {
    auto *resized_image = (unsigned char *) malloc(new_width * new_height * channels);
    stbir_resize_uint8_srgb(image, width, height, 0, resized_image, new_width, new_height, 0, STBIR_RGBA);
    return resized_image;
}