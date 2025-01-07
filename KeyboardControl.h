#ifndef KEYBOARD_CONTROL_H
#define KEYBOARD_CONTROL_H

#include "TankControlInterface.h"
#include <Bluepad32.h>

class KeyboardControl : public TankControlInterface {
public:
    KeyboardControl();
    ~KeyboardControl();

    void update() override;

    int getLeftTrackSpeed() const override;
    int getRightTrackSpeed() const override;

    int getTurretRotation() const override;
    int getTurretElevation() const override;

    bool isFlamethrowerActive() const override;

    int getCurrentGear() const override;

private:
    // Internal methods and variables
    static void onConnectedController(ControllerPtr ctl);
    static void onDisconnectedController(ControllerPtr ctl);

    void processKeyboard(ControllerPtr ctl);

    // Controller instance
    ControllerPtr keyboardController;

    // Control variables
    int currentGear;
    int leftTrackSpeed;
    int rightTrackSpeed;
    int turretRotation;
    int turretElevation;
    bool flamethrowerActive;

    // Static instance for callbacks
    static KeyboardControl* instance;

    // Key states
    bool wPressed;
    bool sPressed;
    bool aPressed;
    bool dPressed;
    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;
    bool ePressed;
    bool qPressed;
    bool spacePressed;

    // Timing variables for smooth control
    unsigned long lastUpdateTime;
    const unsigned long updateInterval = 50; // in milliseconds
};

#endif // KEYBOARD_CONTROL_H
