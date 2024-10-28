/*
Using The runner:
Prereq:
Create the ControllerRunner

mail loop:
2. Set the EffectCurser to the showStart_time - current_time - totalSensorRunTime to get the current time in the show without the sensors runtime
3. Set the mode to 'effect' until a sensor is triggered
3a. If mode is effect and no sensor is clicked, 
    run getNextShowFrame to get current effect at cursor and "frame" (AKA Cursor)
3b. If mode is sensor:
    \\sensorCursor is already init to 0 at ControllerRunner
    Set sensorStartTime
    Set setActiveSensorId
    Inner Sensor Loop 
        while no other sensor is pressed:
            run getNextShowFrame to get current sensor effect at curser and frame
            Set sensorCursor to sensorStartTime - current_time
            IF sensor is complete: 
                set totalSensorRunTime += sensorCursor
                reset sensorCursor to 0
                return to main loop (AkA set mode to effect)
            IF sensor is interupted by other sensor: (double check this logic)
                set totalSensorRunTime += sensorCursor
                reset sensorCursor to 0
                return to '3b loop' or 'main loop' depending on correct logic

sensorCurser and effectCurser are independent so the state of effectCurser isnt effected by sensors adn preserves 

*/

#include "ControllerRunner.h"
#include <iostream>

ControllerRunner::ControllerRunner(const Show &newShow)
    : show(newShow), effectCursor(), sensorCursor(0)
{
    std::cout << "ControllerRunner initialized with cursor set to " << effectCursor << std::endl;
}

void ControllerRunner::setEffectCursor(unsigned long newCursor)
{
    effectCursor = newCursor;
    std::cout << "Cursor updated to " << effectCursor << std::endl;
}

unsigned long ControllerRunner::getEffectCursor() const
{
    return effectCursor;
}

void ControllerRunner::setSensorCursor(unsigned long newCursor)
{
    sensorCursor = newCursor;
    std::cout << "Sensor Cursor updated to " << effectCursor << std::endl;
}

unsigned long ControllerRunner::getSensorCursor() const
{
    return sensorCursor;
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

void ControllerRunner::setTotalSensorRuntime(unsigned long runtime) {
    totalSensorsRuntime = runtime;
    std::cout << "Total sensor runtime updated to " << totalSensorsRuntime << std::endl;
}

unsigned long ControllerRunner::getTotalSensorRuntime() const {
    return totalSensorsRuntime;
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
            if (e->startTimeMs <= effectCursor && effectCursor < e->startTimeMs + e->durationMs)
            {
                effects.push_back(e);
            }
        }
    currentEffectShowFrame = ShowFrame{effects, effectCursor};    
    }
    else // if (mode == "sensor")
    {
        std::cout << "Handling sensor mode" << std::endl;
        Sensor* s = show.sensors.at(getActiveSensorId());
       
       // Eleen ToDo:
       // effects.push_back(s->effect) but sensor doesnt have an Effect object so maybe imolemnt the following

       switch (s->id)
       {
       case 0:
        // new effect = 'rainbow'
        break;
       
       default:
        break;
       }
        currentSensorShowFrame = ShowFrame{effects, sensorCursor};  
    }
}
