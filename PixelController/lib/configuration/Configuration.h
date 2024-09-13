//
// Created by Nick Vazquez on 9/11/24.
//

#ifndef PIXELCONTROLLER_CONFIGURATION_H
#define PIXELCONTROLLER_CONFIGURATION_H

#include <utility>

#include "../../include/json.hpp"
#include "Pair.h"


enum SensorType {
    BINARY = 1,
    ANALOG = 2
};

class Sensor {
public:
    int id;
    int pin;
    SensorType type;
    Pair *location;

    Sensor(int id, int pin, SensorType type, Pair *location) {
        this->id = id;
        this->pin = pin;
        this->type = type;
        this->location = location;
    }

    static Sensor from_json(const nlohmann::json &j);
};


class Effect {
public:
    int id;
    std::string *name;
    Pair *origin;
    Pair *size;
    double startTimeMs;
    double durationMs;
    Translation *translation;

    Effect(int id, std::string *name, Pair *origin, Pair *size, double startTimeMs, double durationMs,
           Translation *translation) {
        this->id = id;
        this->name = name;
        this->origin = origin;
        this->size = size;
        this->startTimeMs = startTimeMs;
        this->durationMs = durationMs;
        this->translation = translation;
    }

    static Effect *from_json(const nlohmann::json &j);
};

#endif //PIXELCONTROLLER_CONFIGURATION_H
