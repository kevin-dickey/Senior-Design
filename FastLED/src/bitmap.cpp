#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {

    // pasrse the command line arguments
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <image_path>" << endl;
        return -1;
    }

    // Load an image
    Mat image = imread(argv[1]);

    if (image.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    // Convert the image to a grayscale image
    Mat grayImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);

    // Convert the grayscale image to a pixel matrix
    vector<vector<int>> pixelMatrix;
    for (int i = 0; i < grayImage.rows; ++i) {
        vector<int> row;
        for (int j = 0; j < grayImage.cols; ++j) {
            row.push_back(static_cast<int>(grayImage.at<uchar>(i, j)));
        }
        pixelMatrix.push_back(row);
    }

    // Output the pixel matrix (for demonstration purposes)
    for (const auto& row : pixelMatrix) {
        for (int pixel : row) {
            cout << pixel << " ";
        }
        cout << endl;
    }

    return 0;
}

