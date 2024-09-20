#ifndef PIXELCONTROLLER_SENSOR_H
#define PIXELCONTROLLER_SENSOR_H

#include <iostream>
#include <vector>
#include <Arduino.h>

#include "esp_attr.h"

#include "../../include/json.hpp"
#include "Spatials.h"

enum SensorType {
    S_BINARY = 1,
    S_ANALOG = 2
};
    
void IRAM_ATTR onSensorTriggered(void *arg);

class Sensor {
public:
    int id;
    int pin;
    SensorType type;
    Pair location;

    inline Sensor(int id, int pin, SensorType type, Pair location) {
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
                Pair_t::from_json(j["location"])
        };
    }
};



class SensorManager {
public:
    SensorManager() = default;
    ~SensorManager() = default;

    void setSensors(std::vector<Sensor *> sensors);

    std::vector<Sensor *> getSensors();
    std::vector<bool> getSensorStates(bool reset = false);

private:
    std::vector<Sensor *> sensors;
    std::vector<bool> sensorStates;

    void removeSensorInterrupts(std::vector<Sensor *> sensors);
    void addSensorInterrupts(std::vector<Sensor *> sensors);

};

#endif // PIXELCONTROLLER_SENSOR_H