#ifndef SERIAL_CONTROL_H
#define SERIAL_CONTROL_H

#include "TankControlInterface.h"
#include <Arduino.h>

class SerialControl : public TankControlInterface {
public:
    SerialControl();
    ~SerialControl();

    void update() override;

    bool isConnected() const override;

    int getLeftTrackSpeed() const override;
    int getRightTrackSpeed() const override;

    int getTurretRotation() const override;
    int getTurretElevation() const override;

    bool isFlamethrowerActive() const override;

    int getCurrentGear() const override;

private:
    // Control variables
    int currentGear;
    int leftTrackSpeed;
    int rightTrackSpeed;
    int turretRotation;
    int turretElevation;
    bool flamethrowerActive;

    // Timing variables for smooth control
    unsigned long lastUpdateTime;
    const unsigned long updateInterval = 50; // in milliseconds

    // State variables for controls
    bool forwardPressed;
    bool backPressed;
    bool leftPressed;
    bool rightPressed;
    bool turretLeftPressed;
    bool turretRightPressed;
    bool turretElevatePressed;
    bool turretLowerPressed;
    bool firePressed;

    // Methods
    void processSerialInput();
    void updateControlVariables();
};

#endif // SERIAL_CONTROL_H
