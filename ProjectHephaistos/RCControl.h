#ifndef RC_CONTROL_H
#define RC_CONTROL_H

#include "TankControlInterface.h"
#include <Arduino.h>

class RCControl : public TankControlInterface {
public:
    RCControl();
    ~RCControl();

    void update() override;

    bool isConnected() const override;

    int getLeftTrackSpeed() const override;
    int getRightTrackSpeed() const override;

    int getTurretRotation() const override;
    int getTurretElevation() const override;

    bool isFlamethrowerActive() const override;

    int getCurrentGear() const override;

private:
    // Pins for RC receiver signals
    static const int throttlePin       = 2; // Movement forward/back
    static const int steeringPin       = 3; // Left/right turning
    static const int gearPin           = 4; // Gear shifting
    static const int turretRotationPin = 5; // Turret left/right
    static const int turretElevationPin= 6; // Turret up/down
    static const int firePin           = 7; // Flamethrower

    // Control variables
    int currentGear;
    int leftTrackSpeed;
    int rightTrackSpeed;
    int turretRotation;
    int turretElevation;
    bool flamethrowerActive;

    // Internal reading method
    void readRCInputs();
};

#endif // RC_CONTROL_H
