#include "Sensor.h"

void SensorManager::setSensors(std::vector<Sensor *> sensors)
{
    std::cout << "Setting up " << sensors.size() << " sensors" << std::endl;

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

void IRAM_ATTR onSensorTriggered(){
    // Set the value at the array index passed in to true
    bool triggered = true;
}

void SensorManager::addSensorInterrupts(std::vector<Sensor *> sensors)
{
    for (auto &sensor : sensors)
    {
        pinMode(sensor->pin, INPUT_PULLUP);
        auto sensor_id = sensor->id;
        attachInterrupt(12, onSensorTriggered, FALLING);
    }
}

std::vector<Sensor *> SensorManager::getSensors()
{
    return this->sensors;
}

std::vector<bool> SensorManager::getSensorStates(bool reset)
{
    auto states = this->sensorStates;
    if (reset)
    {
        this->sensorStates.assign(this->sensorStates.size(), false);
    }
    return states;
}