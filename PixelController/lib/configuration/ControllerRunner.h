#ifndef PIXELCONTROLLER_CONTROLLERRUNNER_H
#define PIXELCONTROLLER_CONTROLLERRUNNER_H

#include "./Configuration.h"
#include <vector>

class ControllerRunner
{
public:
    struct ShowFrame
    {
        std::vector<Effect*, unsigned long> showFrames;
        //int is the frame # which is equivilant to effect StartTime - cursor
    };

    // Constructor taking a reference or pointer to a Show object
    explicit ControllerRunner(const Show &show, const unsigned long showStartTimeMS);

    ~ControllerRunner();

    ShowFrame getNextShowFrame();
    void setEffectCursor();
    unsigned long getEffectCursor() const;
    void setSensorCursorStart(unsigned long newCursor);
    void setSensorCursor();
    void setMode(const std::string& newMode);
    std::string getMode() const;
    void setActiveSensorId(int newActiveSensorId);
    int getActiveSensorId() const;
    void setTotalSensorRuntime(unsigned long runtime);
    unsigned long getTotalSensorRuntime() const;
    void setSensors(std::vector<bool> sensor_states);
    std::vector<bool> getSensors() const;
    boolean checkSensors();

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
};

#endif // PIXELCONTROLLER_CONTROLLERRUNNER_H
