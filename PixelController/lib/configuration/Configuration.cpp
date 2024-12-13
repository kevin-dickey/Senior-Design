//
// Created by Nick Vazquez on 9/17/24.
//

// This file is merely here to help the linker know that this is a C++ library.
// It is not meant to be included in any other file.
// Adding includes for library files also adds them to the linker's list of files to validate & compile.
#include "Configuration.h"

// adding this here to fix some linker issues caused by trying to keep the second loadShow func w/ the filepath :*
#if USE_EMULATOR
Show_t loadShow(const std::string &filename)
{
    std::ifstream f3(filename);
    nlohmann::json data3 = nlohmann::json::parse(f3);
    Show show = Show::from_json(data3);
    f3.close();

    std::cout << "Show Name: " << show.name << std::endl;
    std::cout << "Show Duration: " << show.duration << std::endl;

    return show;
}
#else
Show_t loadShow(File sdFile)
{
    // Check if the file is open
    if (!sdFile)
    {
        throw std::runtime_error("Failed to open file");
    }

    // Determine the size of the file
    size_t fileSize = sdFile.size();
    std::cout << "File size: " << fileSize << " bytes" << std::endl;

    // Allocate a buffer to hold the file contents
    std::vector<uint8_t> buffer(fileSize);

    // Read the file into the buffer
    size_t bytesRead = sdFile.read(buffer.data(), fileSize);
    if (bytesRead != fileSize)
    {
        throw std::runtime_error("Failed to read the entire file");
    }

    std::cout << "Read " << bytesRead << " bytes" << std::endl;

    // Parse the buffer with the JSON library
    nlohmann::json data;
    try
    {
        data = nlohmann::json::parse(buffer.begin(), buffer.end());
#ifdef DEBUG_MODE
        std::cout << "Parsed JSON: " << data.dump(4) << std::endl;
#endif
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        throw;
    }

    Show_t show;
    try
    {
        show = Show::from_json(data);
        return show;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error deserializing show: " << e.what() << std::endl;
        throw;
    }
}
#endif

