// File: FileManager.h
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <stdexcept>

#ifdef USE_EMULATOR
#else
// Running on the ESP32
#include <SPI.h>
#include <SD.h>

// Custom SPI pins for the ESP32
#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCK 18
#define VSPI_SS 16 

// Required for the SD library to work. Cannot be overriden by function arguments.
#define SPI_MISO_PIN VSPI_MISO
#define SPI_MOSI_PIN VSPI_MOSI
#define SPI_SCK_PIN VSPI_SCK

#endif

class FileManager
{
public:
    bool mounted = false;

    FileManager() = default;
    ~FileManager();
    bool MountFileSystem();
    void printCardInformation();

#if USE_EMULATOR
#else
    void printDirectory(File dir, int numTabs, bool showHiddenItems = false);
    SDFile getJsonFile(const std::string &filename);

private:
    SDFile root;
#endif
};

#endif // FILEMANAGER_H