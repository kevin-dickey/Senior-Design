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

ControllerRunner::ControllerRunner(const Show &newShow, const unsigned long showStartTimeMS)
    : show(newShow), effectCursor(0), sensorCursor(0), showStartTimeMS(showStartTimeMS), totalSensorsRuntime(0)
{
    mode = "effect";
    std::cout << "ControllerRunner initialized with effect cursor set to: " << effectCursor << ". Sensor Cursor set to: " << sensorCursor << ". Mode set to: " << mode << std::endl;
}

void ControllerRunner::setEffectCursor()
{
    effectCursor = millis() -showStartTimeMS-  totalSensorsRuntime;
    std::cout << "Cursor updated to " << effectCursor << std::endl;
}

unsigned long ControllerRunner::getEffectCursor() const
{
    return effectCursor;
}

void ControllerRunner::setSensorCursorStart(unsigned long newCursor)
{
    sensorCursorStart = newCursor;
    std::cout << "Sensor Cursor start updated to " << effectCursor << std::endl;
}

void ControllerRunner::setSensorCursor()
{
    sensorCursor = millis() - sensorCursorStart;
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

void ControllerRunner::setActiveSensorId(int newActiveSensorId)
{
    activeSensorId = newActiveSensorId;
    std::cout << "ActiveSensorId updated to " << activeSensorId << std::endl;
}

int ControllerRunner::getActiveSensorId() const
{
    return activeSensorId;
}

void ControllerRunner::setTotalSensorRuntime(unsigned long runtime)
{
    totalSensorsRuntime =+ runtime;
    std::cout << "Total sensor runtime updated to " << totalSensorsRuntime << std::endl;
}
unsigned long ControllerRunner::getTotalSensorRuntime() const
{
    return totalSensorsRuntime;
}

void ControllerRunner::setSensors(std::vector<bool> sensor_states)
{
    sensorsStates = sensor_states;
    std::cout << "Sensor States: [";
    for (size_t i = 0; i < sensorsStates.size(); ++i)
    {
        std::cout << (sensorsStates[i] ? "true" : "false");
        if (i < sensorsStates.size() - 1)
        {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

std::vector<bool> ControllerRunner::getSensors() const
{
    return sensorsStates;
}

// ToDo Eleen:
// will this only run the first sensor if multiple sensors are clicked together? how to make sure all sensors get a turn?
boolean ControllerRunner::checkSensors()
{
    for (size_t i = 0; i < sensorsStates.size(); ++i)
    {
        if (sensorsStates[i])
        {
            //if there isnt an active sensor OR new active sensor is not the current active sensor OR the sensor ran its whole duration, reset sensor start time
            //else only update sensor cursor without reseting start time
            if(!activeSensorId || activeSensorId != i || show.sensors.at(activeSensorId)->duration < sensorCursor){
                setSensorCursorStart(millis());
                setSensorCursor();
            }
            else{ 
                setSensorCursor();
            }
            setActiveSensorId(i);
            setMode("sensor");

            return true;
        }
    }

    //total sensor runtime should only be updated right after a sensor is done executing and right before mode is back to effect
    if(activeSensorId != NULL){
        setTotalSensorRuntime(show.sensors.at(activeSensorId)->duration);
    }

    setActiveSensorId(NULL);
    return false;
}

// Tracks show and frames
ControllerRunner::ShowFrame ControllerRunner::getNextShowFrame()
{
    // ToDo Eleen:
    // find the correct frame value for effects and sensors

    //checks the new sensors states
    //Assumes theres external logic deactivating sensors once their runtime is over
    boolean sensorActive = checkSensors();    

    if (!sensorActive)
    {
        // set the time lapsed since start time for the show excluding sensors runtime
        setEffectCursor();
        setMode("effect");
    }

    if (mode == "effect")
    {
        ShowFrame newEffectShowFrame;

        std::cout << "Handling effect mode" << std::endl;
        // itterate to fine current effects for cursor time
        for (Effect *e : show.effects)
        {
            if (e->startTimeMs <= effectCursor && effectCursor < e->startTimeMs + e->durationMs)
            {
                newEffectShowFrame.showFrames.emplace_back(e, effectCursor);
            }
        }
        currentEffectShowFrame = newEffectShowFrame;
        return currentEffectShowFrame;
    }
    else // if (mode == "sensor")
    {
        ShowFrame newSensorShowFrame;
        std::cout << "Handling sensor mode" << std::endl;
        Effect *sensorEffect = show.sensors.at(activeSensorId)->effect;
        newSensorShowFrame.showFrames.emplace_back(sensorEffect, effectCursor);
        currentSensorShowFrame = newSensorShowFrame;
        return currentSensorShowFrame;
    }
}
