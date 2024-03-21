#include <opencv2/opencv.hpp>

// 1) Load the image from a file. I haven't done this in C++ but there is probably a library for it since the language is very robust
// 2) Resize the image to fit the Matrix
// 3) Convert to bitmap by extracting the RGB pixel values

// implement function to load bmp from image out put bmp datatype
void loadBMP(const char *filename, BMP &bmp) {
  FILE *f = fopen(filename, "rb");
  if (f == NULL) {
    printf("Error: Unable to open file %s\n", filename);
    return;
  }
  bmp = BMP(f);
  fclose(f);
}

// load jpg from file
void loadJPG(const char *filename, JPG &jpg) {
  FILE *f = fopen(filename, "rb");
  if (f == NULL) {
    printf("Error: Unable to open file %s\n", filename);
    return;
  }
  jpg = JPG(f);
  fclose(f);
}