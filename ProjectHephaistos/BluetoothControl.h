#ifndef BLUETOOTH_CONTROL_H
#define BLUETOOTH_CONTROL_H

#include "TankControlInterface.h"
#include <Bluepad32.h>

class BluetoothControl : public TankControlInterface {
public:
    BluetoothControl();
    ~BluetoothControl();

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

    void processGamepad(ControllerPtr ctl);

    // Controller instance
    ControllerPtr controller;

    // Control variables
    int currentGear;
    int leftTrackSpeed;
    int rightTrackSpeed;
    int turretRotation;
    int turretElevation;
    bool flamethrowerActive;

    // Static instance for callbacks
    static BluetoothControl* instance;
};

#endif // BLUETOOTH_CONTROL_H
