#ifndef PIXELCONTROLLER_SENSOR_H
#define PIXELCONTROLLER_SENSOR_H

#include "../../include/json.hpp"
#include "Spatials.h"

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

    virtual ~Sensor() = default;

    static Sensor* from_json(const nlohmann::json& j) {
        return new Sensor{
                j["id"],
                j["pin"],
                j["type"],
                Pair::from_json(j["location"])
        };
    }
};

#endif // PIXELCONTROLLER_SENSOR_H