//
// Created by Nick Vazquez on 10/15/24.
//

#include <FS.h>
#include <iostream>
#include "memory.h"
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

// Function to convert fs::File* to FILE*
unsigned char *ImageProcessing::convertFsFileToBuffer(fs::File *fsFile, size_t& fileSize)
{
    if (!fsFile || !*fsFile)
    {
        return nullptr;
    }

    // Get the size of the file
    fileSize = fsFile->size();
    if (fileSize == 0)
    {
        return nullptr;
    }

    // Allocate a buffer to hold the file contents
    uint8_t *buffer = (uint8_t *)malloc(fileSize);
    if (!buffer)
    {
        return nullptr;
    }

    // Read the file into the buffer
    fsFile->read(buffer, fileSize);

    return buffer;
}

int ImageProcessing::get_image_dimensions(const char *filename, int *width, int *height, int *channels)
{
    return stbi_info(filename, width, height, channels);
}

int ImageProcessing::get_image_dimensions_from_memory(unsigned char *buffer, size_t &len, int *outWidth, int *outHeight, int *outChannels)
{
    int res = stbi_info_from_memory(buffer, (int) len, outWidth, outHeight, outChannels);
    if (res == 0)
    {
        std::cerr << "Error getting image dimensions from memory" << std::endl;
        std::cerr << "Error: " << stbi_failure_reason() << std::endl;
    }
    return res;
}

unsigned char *ImageProcessing::load_image(const char *filename, int *width, int *height, int *channels)
{
    return stbi_load(filename, width, height, channels, 0);
}

unsigned char *ImageProcessing::load_image_from_memory(unsigned char *buffer, size_t &len, int *width, int *height, int *channels)
{
    unsigned char *imgData = stbi_load_from_memory(buffer, len, width, height, channels, 0);
    if (!imgData)
    {
        std::cerr << "Error loading image from memory" << std::endl;
        std::cerr << "Error: " << stbi_failure_reason() << std::endl;
    }
    return imgData;
}

void ImageProcessing::free_image(unsigned char *data)
{
    stbi_image_free(data);
}

unsigned char *ImageProcessing::resize_image(const unsigned char *image, int width, int height, int channels, int &new_width, int &new_height, bool preserve_ratio)
{
    if (preserve_ratio)
    {
        float aspect_ratio = static_cast<float>(width) / height;
        if (new_width / aspect_ratio <= new_height)
        {
            new_height = static_cast<int>(new_width / aspect_ratio);
        }
        else
        {
            new_width = static_cast<int>(new_height * aspect_ratio);
        }
    }

    auto *resized_image = (unsigned char *)malloc(new_width * new_height * channels);
    if (!resized_image)
    {
        std::cerr << "Error allocating memory for resized image" << std::endl;
        std::cerr << "Error: " << stbi_failure_reason() << std::endl;
        return nullptr;
    }
    stbir_resize_uint8_srgb(image, width, height, 0, resized_image, new_width, new_height, 0, STBIR_RGBA);
    return resized_image;
}