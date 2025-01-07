#ifndef TANK_CONTROL_INTERFACE_H
#define TANK_CONTROL_INTERFACE_H

class TankControlInterface {
public:
    virtual ~TankControlInterface() {}

    // Methods to update control inputs
    virtual void update() = 0;

    // Getters for movement
    virtual int getLeftTrackSpeed() const = 0;
    virtual int getRightTrackSpeed() const = 0;

    // Getters for turret control
    virtual int getTurretRotation() const = 0;
    virtual int getTurretElevation() const = 0;

    // Getter for flamethrower activation
    virtual bool isFlamethrowerActive() const = 0;

    // Getter for current gear
    virtual int getCurrentGear() const = 0;
};

#endif // TANK_CONTROL_INTERFACE_H
