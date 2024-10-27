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
        int frame;
    };

    // Constructor taking a reference or pointer to a Show object
    explicit ControllerRunner(const Show &show);

    ~ControllerRunner();

    ShowFrame getNextShowFrame();
    void setCursor(int newCursor);
    int getCursor() const;
    void setMode(const std::string& newMode);
    void setActiveSensorId(int newActiveSensorId);
    int getActiveSensorId() const;
    std::string getMode() const;

private:
    int cursor;
    const Show& show;
    std::string mode;
    int activeSensorId;
    ShowFrame currentEffectShowFrame;
    ShowFrame currentSensorShowFrame;
};

#endif // PIXELCONTROLLER_CONTROLLERRUNNER_H
