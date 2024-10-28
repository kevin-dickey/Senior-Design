#ifndef PIXELCONTROLLER_CONTROLLERRUNNER_H
#define PIXELCONTROLLER_CONTROLLERRUNNER_H

#include "./Configuration.h"
#include <vector>

class ControllerRunner
{
public:
    struct ShowFrame
    {
        std::vector<Effect*> effects;
        unsigned long frame;
    };

    // Constructor taking a reference or pointer to a Show object
    explicit ControllerRunner(const Show &show);

    ~ControllerRunner();

    ShowFrame getNextShowFrame();
    void setEffectCursor(unsigned long newCursor);
    unsigned long getEffectCursor() const;
    void setSensorCursor(unsigned long newCursor);
    unsigned long getSensorCursor() const;
    void setMode(const std::string& newMode);
    std::string getMode() const;
    void setActiveSensorId(int newActiveSensorId);
    int getActiveSensorId() const;
    void setTotalSensorRuntime(unsigned long runtime);
    unsigned long getTotalSensorRuntime() const;

private:
    unsigned long effectCursor;
    unsigned long sensorCursor;
    unsigned long totalSensorsRuntime;
    const Show& show;
    std::string mode;
    int activeSensorId;
    ShowFrame currentEffectShowFrame;
    ShowFrame currentSensorShowFrame;

};

#endif // PIXELCONTROLLER_CONTROLLERRUNNER_H
