#ifndef PIXELCONTROLLER_CONTROLLERRUNNER_H
#define PIXELCONTROLLER_CONTROLLERRUNNER_H

#include "./Configuration.h"
#include <vector>
#include <chrono>


class ControllerRunner
{
public:


    struct ShowFrame
    {
        Effect* effect;
        int frame;
        //int is the frame # which is equivilant to effect StartTime - cursor
    };

    // Constructor taking a reference or pointer to a Show object
    explicit ControllerRunner(const Show &show, const unsigned long showStartTimeMS, std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1,1000000000>>>epoch);

    ~ControllerRunner();

    ShowFrame getNextShowFrame(std::vector<bool> sensor_states);
    void setEffectCursor();
    unsigned long getEffectCursor() const;
    void setSensorCursorStart();
    void setSensorCursor();
    void setMode(const std::string& newMode);
    std::string getMode() const;
    void setActiveSensorId(int newActiveSensorId);
    int getActiveSensorId() const;
    void setTotalSensorRuntime(unsigned long runtime);
    unsigned long getTotalSensorRuntime() const;
    void setSensors(std::vector<bool> sensor_states);
    std::vector<bool> getSensors() const;
    bool checkSensors();
    unsigned long getMillis() const;

private:
    const unsigned long showStartTimeMS;
    unsigned long effectCursor;
    unsigned long sensorCursorStart;
    unsigned long sensorCursor;
    unsigned long totalSensorsRuntime;
    const Show& show;
    std::string mode;
    int activeSensorId;
    ShowFrame currentEffectShowFrame;
    ShowFrame currentSensorShowFrame;
    std::vector<bool> sensorsStates;
    std::vector<ShowFrame> showFrames;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1,1000000000>>>epoch;

    void resetSensor();
};

#endif // PIXELCONTROLLER_CONTROLLERRUNNER_H
