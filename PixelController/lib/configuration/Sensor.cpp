#include "Sensor.h"

std::vector<bool> triggeredInterrupts;
std::vector<long> sensorLastTriggeredMillis;
std::vector<long> interruptReadMillis;

void SensorManager::setSensors(std::vector<Sensor *> sensors)
{
#if USE_EMULATOR
    std::cout << "🛑 Skipping sensor setup in emulator." << std::endl;
    std::cout << "   All sensors will be set to never trigger." << std::endl;
    return;
#else
    // TODO: Set up a better logging system which redirects cout to the serial monitor.
    Serial.println("Setting up " + String(sensors.size()) + " sensors");

    this->removeSensorInterrupts(this->sensors);

    this->sensors = sensors;
    triggeredInterrupts.resize(sensors.size());
    triggeredInterrupts.assign(sensors.size(), false);

    sensorLastTriggeredMillis.resize(sensors.size());
    sensorLastTriggeredMillis.assign(sensors.size(), 0);

    interruptReadMillis.resize(sensors.size());
    interruptReadMillis.assign(sensors.size(), 0);

    this->addSensorInterrupts(this->sensors);
#endif
}

void SensorManager::removeSensorInterrupts(std::vector<Sensor *> sensors)
{
    for (auto &sensor : sensors)
    {
        detachInterrupt(sensor->pin);
    }
}

void IRAM_ATTR onSensorTriggered(void *arg)
{
    // Set the value at the array index passed in to true
    uint32_t sensor_id = reinterpret_cast<uint32_t>(arg);
    triggeredInterrupts[sensor_id] = true;
    interruptReadMillis[sensor_id] = millis();
}

void SensorManager::addSensorInterrupts(std::vector<Sensor *> sensors)
{
    for (auto &sensor : sensors)
    {
        pinMode(sensor->pin, INPUT_PULLUP);
        auto sensor_id = sensor->id;
        Serial.println("Attaching interrupt for sensor " + String(sensor_id) + " on pin " + String(sensor->pin));
        attachInterruptArg(digitalPinToInterrupt(sensor->pin), onSensorTriggered, reinterpret_cast<void *>(sensor->id), FALLING);
    }
}

std::vector<Sensor *> SensorManager::getSensors()
{
    return this->sensors;
}

std::vector<bool> SensorManager::getSensorStates(bool reset)
{
#if USE_EMULATOR
    // Make a vector with the correct size. We currently don't set the number of sensors in the emulator.
    std::cout << "🛑 Skipping sensor state retrieval in emulator." << std::endl;
    return std::vector<bool>{};
#else
    auto states = triggeredInterrupts;

    // If the last triggered time is greater than the debounce time, reset the sensor state
    // ignore the input if the sensor was triggered less than SENSOR_DEBOUNCE_MS ago
    for (int i = 0; i < states.size(); i++)
    {
        if (states[i])
        {
            if (interruptReadMillis[i] - sensorLastTriggeredMillis[i] > SENSOR_DEBOUNCE_MS)
            {
                sensorLastTriggeredMillis[i] = interruptReadMillis[i];
            }
            else
            {
                Serial.println("Ignoring sensor " + String(i) + " because it was triggered " + String(interruptReadMillis[i] - sensorLastTriggeredMillis[i]) + "ms ago");
                states[i] = false;
            }
        }
    }

    if (reset)
    {
        triggeredInterrupts.assign(triggeredInterrupts.size(), false);
    }

    return states;
#endif
}