#ifndef BLUETOOTH_CONTROL_H
#define BLUETOOTH_CONTROL_H

#include "TankControlInterface.h"
#include <Arduino.h>
#include <Bluepad32.h>

// -------------------------------
// Define a joystick deadzone
// (Tune this as needed)
// -------------------------------
#define JOYSTICK_DEADZONE 100

class BluetoothControl : public TankControlInterface {
public:
    BluetoothControl();
    ~BluetoothControl();

    void update() override;

    bool isConnected() const override;

    int getLeftTrackSpeed() const override;
    int getRightTrackSpeed() const override;

    int getTurretRotation() const override;
    int getTurretElevation() const override;

    bool isFlamethrowerActive() const override;

    int getCurrentGear() const override;

private:
    // Callback when a new controller is connected
    static void onConnectedController(ControllerPtr ctl);
    // Callback when a controller is disconnected
    static void onDisconnectedController(ControllerPtr ctl);

    void processGamepad(ControllerPtr ctl);
    void updateControlVariables();

    // We'll store the currently connected controller
    static ControllerPtr controller;

    // Internal control variables
    int currentGear;
    int leftTrackSpeed;
    int rightTrackSpeed;
    int turretRotation;
    int turretElevation;
    bool flamethrowerActive;

    // State variables for pressed states
    bool forwardPressed;
    bool backPressed;
    bool leftPressed;
    bool rightPressed;
    bool turretLeftPressed;
    bool turretRightPressed;
    bool turretElevatePressed;
    bool turretLowerPressed;
    bool firePressed;

    // For edge detection on gear shifting
    bool dpadUpPrevious;
    bool dpadDownPrevious;

    // Update timing
    unsigned long lastUpdateTime;
    const unsigned long updateInterval = 50; // in milliseconds

    static BluetoothControl* instance;
};

#endif // BLUETOOTH_CONTROL_H
