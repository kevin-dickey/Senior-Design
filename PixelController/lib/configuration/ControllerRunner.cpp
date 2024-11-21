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
#include <chrono>

ControllerRunner::ControllerRunner(const Show &newShow, const unsigned long showStartTimeMS, std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1,1000000000>>>epoch)
    : show(newShow), effectCursor(0), sensorCursor(0), showStartTimeMS(showStartTimeMS), totalSensorsRuntime(0), epoch(epoch),activeSensorId(-1),sensorCursorStart(0), currentEffectShowFrame(), currentSensorShowFrame()
        {
    mode = "effect";
    std::cout << "ControllerRunner initialized with effect cursor set to: " << effectCursor << ". Sensor Cursor set to: " << sensorCursor << ". Mode set to: " << mode << std::endl;
}

unsigned long ControllerRunner::getMillis() const {
    auto now = std::chrono::high_resolution_clock ::now();
    auto mseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - epoch).count();
    return mseconds;}

void ControllerRunner::setEffectCursor()
{
    effectCursor = getMillis() -showStartTimeMS-  totalSensorsRuntime;
    std::cout << "Cursor updated to " << effectCursor << std::endl;
}

unsigned long ControllerRunner::getEffectCursor() const
{
    return effectCursor;
}

void ControllerRunner::setSensorCursorStart()
{
    sensorCursorStart = getMillis() -showStartTimeMS -  totalSensorsRuntime;
    std::cout << "Sensor Cursor start updated to " << sensorCursorStart << std::endl;
}

void ControllerRunner::setSensorCursor()
{
    sensorCursor = getMillis() -showStartTimeMS- sensorCursorStart;
    std::cout << "Sensor Cursor updated to " << sensorCursor << std::endl;
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
    totalSensorsRuntime += runtime;
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
bool ControllerRunner::checkSensors()
{
    //if sensor was already active
    if(activeSensorId != -1){
        setSensorCursor();
        //if sensor ran it whole duration
        if(sensorCursor >= show.sensors[activeSensorId]->effect->durationMs){
            resetSensor();
            return false;
        }
        //if sensor is still running its duration
        else{
            return true;
        }
    }
    //if sensor was NOT already active
    else{
        //search for which new sensor was triggered
        for (size_t i = 0; i < sensorsStates.size(); ++i)
        {
            //if sensor at i is true/triggered && there is NOT an active sensor (IE only the first triggered sensor is ran)
            if (sensorsStates[i] && activeSensorId!=0)
            {
                setSensorCursorStart();
                setSensorCursor();
                setActiveSensorId(i);
                setMode("sensor");
                return true;
            }
        }
    }

    // there was NOT an active sensor and no new sensors were activated
    setActiveSensorId(-1);
    return false;
}

void ControllerRunner::resetSensor() {
    setTotalSensorRuntime(show.sensors[activeSensorId]->effect->durationMs);
    setActiveSensorId(-1);
    setMode("effect");
    currentSensorShowFrame.effect = nullptr;
    currentSensorShowFrame.frame = -1;
    sensorCursor = 0;
    sensorCursorStart=0;
    currentSensorShowFrame={};
}

// Tracks show and frames
ControllerRunner::ShowFrame ControllerRunner::getNextShowFrame(std::vector<bool> sensor_states)
{
      //checks the new sensors states
    setSensors(sensor_states);
    bool sensorActive = checkSensors();

    if (!sensorActive)
    {
        // set the time lapsed since start time for the show excluding sensors runtime
        setEffectCursor();
        setMode("effect");
    }

    if(show.duration <= effectCursor) {
        Effect* e = show.effects[0];
        currentEffectShowFrame.effect = e;
        currentEffectShowFrame.effect->name = "no effect" ;
        currentEffectShowFrame.frame = -1 ;
        return currentEffectShowFrame;
    }

    showFrames.clear();

    if (mode == "effect")
    {
        ShowFrame newEffectShowFrame;

        std::cout << "Handling effect mode" << std::endl;
        // itterate to fine current effects for cursor time
        for (Effect *e : show.effects) {
            if (e->startTimeMs <= effectCursor && effectCursor < e->startTimeMs + e->durationMs) {
                newEffectShowFrame.effect = e;
                if(currentEffectShowFrame.effect != nullptr && currentEffectShowFrame.effect->name ==  newEffectShowFrame.effect->name) {
                    newEffectShowFrame.frame = currentEffectShowFrame.frame + 1;
                }
                else {
                    newEffectShowFrame.frame = 1;
                }
                showFrames.push_back(newEffectShowFrame);
            }
        }

        if(newEffectShowFrame.frame){
            currentEffectShowFrame = newEffectShowFrame;
            return newEffectShowFrame;
        }
        else{
            currentEffectShowFrame.effect->name = "no effect" ;
            currentEffectShowFrame.frame = -1 ;
            return currentEffectShowFrame;
        }

    }
    else // if (mode == "sensor")
    {
        std::cout << "Handling sensor mode" << std::endl;
        ShowFrame newSensorShowFrame{}; 
        Effect *newSensorEffect = show.sensors.at(activeSensorId)->effect;
        newSensorShowFrame.effect = newSensorEffect;
        if(currentSensorShowFrame.effect != nullptr && currentSensorShowFrame.effect->name ==  newSensorShowFrame.effect->name) {
            newSensorShowFrame.frame = currentSensorShowFrame.frame + 1;
        }
        else {
            newSensorShowFrame.frame = 1;
        }
        showFrames.push_back(newSensorShowFrame);
        currentSensorShowFrame = newSensorShowFrame;
        return currentSensorShowFrame;
    }
}

ControllerRunner::~ControllerRunner() {

}

