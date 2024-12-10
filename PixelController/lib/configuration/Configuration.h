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
    std::vector<Layout *> layouts;
    std::vector<Effect *> effects;
    std::vector<Sensor *> sensors;

    static Show from_json(const nlohmann::json &j)
    {
        Show show;
        try
        {
            show.name = j.at("name").get<std::string>();
            show.duration = j.at("duration").get<double>(); // might be durationMs? not sure.

            std::cout << "Show Name: " << show.name << std::endl;
            std::cout << "Show Duration: " << show.duration << std::endl;

            for (const auto &effect : j["effects"])
            {
                try
                {
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

Show_t loadShow(File sdFile);
Show_t loadShow(const std::string &filename);

#endif // CONFIGURATION_H