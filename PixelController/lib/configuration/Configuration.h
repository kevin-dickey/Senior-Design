// Configuration.h
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include "../../include/json.hpp"
#include "Spatials.h"
#include "Effect.h"
#include "Layout.h"
#include "Sensor.h"


struct Show {
    std::string name;
    double duration;
    std::vector<Layout*> layouts;
    std::vector<Effect*> effects;
    std::vector<Sensor*> sensors;

    static Show from_json(const nlohmann::json& j) {
        Show show;
        show.name = j["name"];
        show.duration = j["duration"];
        for (const auto& layout : j["layouts"]) {
            switch (layout["shape"].get<LayoutType>()) {
                case LayoutType::GRID:
                    show.layouts.push_back(GridLayout::from_json(layout));
                    break;
                default:
                    show.layouts.push_back(Layout::from_json(layout));
                    break;
            }
        }
        for (const auto& effect : j["effects"]) {
            show.effects.push_back(Effect::from_json(effect));
        }
        for (const auto& sensor : j["sensors"]) {
            show.sensors.push_back(Sensor::from_json(sensor));
        }
        return show;
    }

    ~Show() {
        for (auto layout : layouts) { delete layout; }
        for (auto effect : effects) { delete effect; }
        for (auto sensor : sensors) { delete sensor; }
    }
};

Show loadShow(const std::string& filename) {
    std::ifstream f3(filename);
    nlohmann::json data3 = nlohmann::json::parse(f3);
    Show show = Show::from_json(data3);
    f3.close();

    std::cout << "Show Name: " << show.name << std::endl;
    std::cout << "Show Duration: " << show.duration << std::endl;

    return show;
}

#endif // CONFIGURATION_H