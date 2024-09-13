//
// Created by Nick Vazquez on 9/12/24.
//

#ifndef PIXELCONTROLLER_SENSOR_H
#define PIXELCONTROLLER_SENSOR_H

#include "../../include/json.hpp"
#include "Spatials.h"

enum SensorType {
    BINARY = 1,
    ANALOG = 2
};

struct Sensor {
    int id;
    int pin;
    SensorType type;
    Pair location;

    static Sensor *from_json(const nlohmann::json &j) {
        return new Sensor{
            j["id"],
            j["pin"],
            j["type"],
            Pair::from_json(j["location"])
        };
    }
};

#endif //PIXELCONTROLLER_SENSOR_H
