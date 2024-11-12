#ifndef PIXELCONTROLLER_SENSOR_H
#define PIXELCONTROLLER_SENSOR_H

#include <iostream>
#include <vector>

// Only include Arduino.h if not using the emulator
#ifndef USE_EMULATOR
#include <Arduino.h>
#include "esp_attr.h"
#endif

#include "../../include/json.hpp"
#include "Spatials.h"

#define SENSOR_DEBOUNCE_MS 100 // originally 5000

enum SensorType {
    S_BINARY = 1,
    S_ANALOG = 2
};
    
extern std::vector<bool> triggeredInterrupts;
extern std::vector<long> sensorLastTriggeredMillis;

#ifndef USE_EMULATOR
void IRAM_ATTR onSensorTriggered(void *arg);
#endif

// Eleen ToDo:
// Add effect object and serialize it 
class Sensor {
public:
    int id;
    int pin;
    unsigned long duration;
    SensorType type;
    Pair location;
    Effect* effect;

    inline Sensor(int id, int pin, unsigned long duration, SensorType type, Pair location, Effect* effect) {
        this->id = id;
        this->pin = pin;
        this->duration = duration;
        this->type = type;
        this->location = location;
        this->effect = effect;
    }

    virtual ~Sensor() = default;

    static Sensor* from_json(const nlohmann::json& j) {
        return new Sensor{
                j["id"],
                j["pin"],
                j["type"],
                j["duration"],
                Pair_t::from_json(j["location"]),
                Effect::from_json(j["effects"])
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

    void removeSensorInterrupts(std::vector<Sensor *> sensors);
    void addSensorInterrupts(std::vector<Sensor *> sensors);
};

#endif // PIXELCONTROLLER_SENSOR_H