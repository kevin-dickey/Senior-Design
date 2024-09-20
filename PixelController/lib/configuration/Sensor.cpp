#include "Sensor.h"

std::vector<bool> sensorStates;

void SensorManager::setSensors(std::vector<Sensor *> sensors)
{
    // TODO: Set up a better logging system which redirects cout to the serial monitor.
    Serial.println("Setting up " + String(sensors.size()) + " sensors");

    this->removeSensorInterrupts(this->sensors);

    this->sensors = sensors;
    sensorStates.resize(sensors.size());
    sensorStates.assign(sensors.size(), false);

    this->addSensorInterrupts(this->sensors);
}

void SensorManager::removeSensorInterrupts(std::vector<Sensor *> sensors)
{
    for (auto &sensor : sensors)
    {
        detachInterrupt(sensor->pin);
    }
}

void IRAM_ATTR onSensorTriggered(void *arg){
    // Set the value at the array index passed in to true
    sensorStates[reinterpret_cast<uint32_t>(arg)] = true;
}

void SensorManager::addSensorInterrupts(std::vector<Sensor *> sensors)
{
    for (auto &sensor : sensors)
    {
        pinMode(sensor->pin, INPUT_PULLUP);
        auto sensor_id = sensor->id;
        Serial.println("Attaching interrupt for sensor " + String(sensor_id) + " on pin " + String(sensor->pin));
        attachInterruptArg(sensor->pin, onSensorTriggered, reinterpret_cast<void*>(sensor->id), FALLING);
    }
}

std::vector<Sensor *> SensorManager::getSensors()
{
    return this->sensors;
}

std::vector<bool> SensorManager::getSensorStates(bool reset)
{
    auto states = sensorStates;
    if (reset)
    {
        sensorStates.assign(sensorStates.size(), false);
    }
    return states;
}