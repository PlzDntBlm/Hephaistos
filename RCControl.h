#ifndef RC_CONTROL_H
#define RC_CONTROL_H

#include "TankControlInterface.h"

class RCControl : public TankControlInterface {
public:
    RCControl();
    ~RCControl();

    void update() override;

    int getLeftTrackSpeed() const override;
    int getRightTrackSpeed() const override;

    int getTurretRotation() const override;
    int getTurretElevation() const override;

    bool isFlamethrowerActive() const override;

    int getCurrentGear() const override;

private:
    // Internal methods and variables
    void readRCInputs();

    // Control variables
    int currentGear;
    int leftTrackSpeed;
    int rightTrackSpeed;
    int turretRotation;
    int turretElevation;
    bool flamethrowerActive;

    // RC input pins
    static const int channelThrottlePin = 2; // Example pin numbers
    static const int channelSteeringPin = 3;
    static const int channelGearPin = 4;
    static const int channelTurretRotationPin = 5;
    static const int channelTurretElevationPin = 6;
    static const int channelFlamethrowerPin = 7;
};

#endif // RC_CONTROL_H
