#include "ControllerRunner.h"
#include <iostream>

ControllerRunner::ControllerRunner(const Show &newShow)
    : show(newShow), cursor(0)
{
    std::cout << "ControllerRunner initialized with cursor set to " << cursor << std::endl;
}

void ControllerRunner::setCursor(int newCursor)
{
    cursor = newCursor;
    std::cout << "Cursor updated to " << cursor << std::endl;
}

int ControllerRunner::getCursor() const
{
    return cursor;
}

void ControllerRunner::setMode(const std::string &newMode)
{
    mode = newMode;
    std::cout << "Mode updated to " << mode << std::endl;
}

std::string ControllerRunner::getMode() const
{
    return mode;
}

void ControllerRunner::setActiveSensorId(int newActiveSensorId) {
    activeSensorId = newActiveSensorId;
    std::cout << "ActiveSensorId updated to " << activeSensorId << std::endl;
}

int ControllerRunner::getActiveSensorId() const {
    return activeSensorId;
}

// Tracks show and frames
ControllerRunner::ShowFrame ControllerRunner::getNextShowFrame()
{
    std::vector<Effect *> effects = {};
    
    if (mode == "effect")
    {
        std::cout << "Handling effect mode" << std::endl;
        // itterate to fine current effects for cursor time
        for (Effect *e : show.effects)
        {
            if (e->startTimeMs <= cursor && cursor < e->startTimeMs + e->durationMs)
            {
                effects.push_back(e);
            }
        }
    currentEffectShowFrame = ShowFrame{effects,cursor};    
    }
    else // if (mode == "sensor")
    {
        std::cout << "Handling sensor mode" << std::endl;
        Sensor* s = show.sensors.at(getActiveSensorId());
       // Eleen ToDo:
       // effects.push_back(s->effect) but sensor doesnt have an Effect object
        currentSensorShowFrame = ShowFrame{effects, cursor};  
    }
}
