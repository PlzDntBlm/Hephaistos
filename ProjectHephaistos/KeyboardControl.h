#ifndef KEYBOARD_CONTROL_H
#define KEYBOARD_CONTROL_H

#include "TankControlInterface.h"
#include <Arduino.h>
#include <Bluepad32.h> // to detect keyboard events

class KeyboardControl : public TankControlInterface {
public:
    KeyboardControl();
    ~KeyboardControl();

    void update() override;

    bool isConnected() const override;

    int getLeftTrackSpeed() const override;
    int getRightTrackSpeed() const override;

    int getTurretRotation() const override;
    int getTurretElevation() const override;

    bool isFlamethrowerActive() const override;

    int getCurrentGear() const override;

private:
    static void onConnectedController(ControllerPtr ctl);
    static void onDisconnectedController(ControllerPtr ctl);

    void processKeyboard(ControllerPtr ctl);
    void updateControlVariables();

    static ControllerPtr keyboardController;
    static KeyboardControl* instance;

    // Control variables
    int currentGear;
    int leftTrackSpeed;
    int rightTrackSpeed;
    int turretRotation;
    int turretElevation;
    bool flamethrowerActive;

    // State variables
    bool forwardPressed;
    bool backPressed;
    bool leftPressed;
    bool rightPressed;
    bool turretLeftPressed;
    bool turretRightPressed;
    bool turretElevatePressed;
    bool turretLowerPressed;
    bool firePressed;

    unsigned long lastUpdateTime;
    const unsigned long updateInterval = 50; // ms
};

#endif // KEYBOARD_CONTROL_H
