// Configuration.h
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <fstream>
#include <string>
#include "json.hpp"
#include "Spatials.h"
#include "Effect.h"
#include "Layout.h"
#include "Sensor.h"


typedef struct Show {
    std::string name;
    double duration;
    std::vector<std::unique_ptr<Layout>> layouts;
    std::vector<std::unique_ptr<Effect>> effects;
    std::vector<std::unique_ptr<Sensor>> sensors;

    static Show from_json(const nlohmann::json& j) {
        Show show;
        show.name = j["name"];
        show.duration = j["duration"];
        for (const auto& layout : j["layouts"]) {
            switch (layout["shape"].get<LayoutType>()) {
                case LayoutType::GRID:
                    show.layouts.push_back(std::unique_ptr<GridLayout>(GridLayout::from_json(layout)));
                    break;
                default:
                    show.layouts.push_back(std::unique_ptr<Layout>(Layout::from_json(layout)));
                    break;
            }
        }
        for (const auto& effect : j["effects"]) {
            show.effects.push_back(std::unique_ptr<Effect>(Effect::from_json(effect)));
        }
        for (const auto& sensor : j["sensors"]) {
            show.sensors.push_back(std::unique_ptr<Sensor>(Sensor::from_json(sensor)));
        }
        return show;
    }
} Show_t;

Show_t loadShow(const std::string& filename) {
    std::ifstream f3(filename);
    nlohmann::json data3 = nlohmann::json::parse(f3);
    Show show = Show::from_json(data3);
    f3.close();

    std::cout << "Show Name: " << show.name << std::endl;
    std::cout << "Show Duration: " << show.duration << std::endl;

    return show;
}

#endif // CONFIGURATION_H