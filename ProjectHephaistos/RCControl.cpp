#include "RCControl.h"
#include <Arduino.h>

RCControl::RCControl()
    : currentGear(1),
      leftTrackSpeed(0),
      rightTrackSpeed(0),
      turretRotation(0),
      turretElevation(0),
      flamethrowerActive(false) {
    // Initialize pins
    pinMode(channelThrottlePin, INPUT);
    pinMode(channelSteeringPin, INPUT);
    pinMode(channelGearPin, INPUT);
    pinMode(channelTurretRotationPin, INPUT);
    pinMode(channelTurretElevationPin, INPUT);
    pinMode(channelFlamethrowerPin, INPUT);
}

RCControl::~RCControl() {
    // Destructor logic if needed
}

void RCControl::update() {
    readRCInputs();
}

void RCControl::readRCInputs() {
    // Read RC channel inputs (pseudo-code)
    int throttleInput = pulseIn(channelThrottlePin, HIGH, 25000); // Read pulse width in microseconds
    int steeringInput = pulseIn(channelSteeringPin, HIGH, 25000);
    int gearInput = pulseIn(channelGearPin, HIGH, 25000);
    int turretRotationInput = pulseIn(channelTurretRotationPin, HIGH, 25000);
    int turretElevationInput = pulseIn(channelTurretElevationPin, HIGH, 25000);
    int flamethrowerInput = pulseIn(channelFlamethrowerPin, HIGH, 25000);

    // Map inputs to control variables
    // Adjust the mapping ranges based on your RC transmitter's configuration

    // Throttle: Typically 1000 to 2000 microseconds
    int forwardBackward = map(throttleInput, 1000, 2000, -100, 100);
    int turn = map(steeringInput, 1000, 2000, -100, 100);

    float gearScaling = (float)currentGear / 5;

    leftTrackSpeed = constrain((forwardBackward + turn) * gearScaling, -100, 100);
    rightTrackSpeed = constrain((forwardBackward - turn) * gearScaling, -100, 100);

    // Gear shifting
    int gearValue = map(gearInput, 1000, 2000, 1, 5);
    currentGear = constrain(gearValue, 1, 5);

    // Turret control
    turretRotation = map(turretRotationInput, 1000, 2000, -100, 100);
    turretElevation = map(turretElevationInput, 1000, 2000, -100, 100);

    // Flamethrower activation
    flamethrowerActive = (flamethrowerInput > 1500); // Adjust threshold as needed
}

int RCControl::getLeftTrackSpeed() const {
    return leftTrackSpeed;
}

int RCControl::getRightTrackSpeed() const {
    return rightTrackSpeed;
}

int RCControl::getTurretRotation() const {
    return turretRotation;
}

int RCControl::getTurretElevation() const {
    return turretElevation;
}

bool RCControl::isFlamethrowerActive() const {
    return flamethrowerActive;
}

int RCControl::getCurrentGear() const {
    return currentGear;
}
