// Configuration.h
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <FS.h>
#include <fstream>
#include <string>
#include "json.hpp"
#include "Spatials.h"
#include "Effect.h"
#include "Layout.h"
#include "Sensor.h"
#include <fstream>

typedef struct Show
{
    std::string name;
    double duration;
    std::vector<Layout*> layouts;
    std::vector<Effect*> effects;
    std::vector<Sensor*> sensors;


    static Show from_json(const nlohmann::json &j)
    {
        Show show;
        try
        {
            show.name = j["name"];
            show.duration = j["duration"];

            std::cout << "Show Name: " << show.name << std::endl;
            std::cout << "Show Duration: " << show.duration << std::endl;

            for (const auto &effect : j["effects"])
            {
                try
                {
                    std::cout << "Parsing Effect: " << effect << std::endl;
                    auto effectObj = Effect::from_json(effect);
                    std::cout << "Effect Name: " << effectObj->name << std::endl;
                    show.effects.push_back(effectObj);
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error deserializing effect: " << e.what() << std::endl;
                    // Skip this effect
                }
            }

            std::cout << "Number of effects: " << show.effects.size() << std::endl;

            for (const auto &layout : j["layouts"])
            {
                try
                {
                    // FIXME: We aren't including ID on the webapp export until multiple layouts are supported.
                    // int id = layout.at("id").get<int>();
                    // std::cout << "Layout ID: " << id << std::endl;
                    switch (layout.at("shape").get<LayoutType>())
                    {
                    case LayoutType::GRID:
                    {
                        std::cout << "Grid Layout!" << std::endl;
                        show.layouts.push_back(GridLayout::from_json(layout));
                        break;
                    }
                    default:
                        {
                        std::cout << "Custom Layout!" << std::endl;
                        // std::cout << "Layout ID: " << layout.at("id").get<int>() << std::endl;
                        show.layouts.push_back(Layout::from_json(layout));
                        break;
                        }
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error deserializing layout: " << e.what() << std::endl;
                    // Skip this layout
                }
            }

            for (const auto &sensor : j["sensors"])
            {
                try
                {
                    std::cout << "Sensor ID: " << sensor.at("id").get<int>() << std::endl;
                    show.sensors.push_back(Sensor::from_json(sensor));
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error deserializing sensor: " << e.what() << std::endl;
                    // Skip this sensor
                }
            }
            std::cout << "Successfully deserialized show!" << std::endl;
            return show;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error deserializing show: " << e.what() << std::endl;
            throw new std::runtime_error("Error deserializing show");
            // Handle the error or rethrow it
        }
    }
} Show_t;

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
        std::cout << "Parsed JSON: " << data.dump(4) << std::endl;
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

inline Show_t loadShow(const std::string& filename) {
    std::ifstream f3(filename);
    nlohmann::json data3 = nlohmann::json::parse(f3);
    Show show = Show::from_json(data3);
    f3.close();

    std::cout << "Show Name: " << show.name << std::endl;
    std::cout << "Show Duration: " << show.duration << std::endl;

    return show;
}


#endif // CONFIGURATION_H