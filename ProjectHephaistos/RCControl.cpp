#include "RCControl.h"

RCControl::RCControl()
    : currentGear(1),
      leftTrackSpeed(0),
      rightTrackSpeed(0),
      turretRotation(0),
      turretElevation(0),
      flamethrowerActive(false)
{
    // Initialize pins
    pinMode(throttlePin, INPUT);
    pinMode(steeringPin, INPUT);
    pinMode(gearPin, INPUT);
    pinMode(turretRotationPin, INPUT);
    pinMode(turretElevationPin, INPUT);
    pinMode(firePin, INPUT);

    Serial.println("RCControl Initialized. Reading RC signals...");
}

RCControl::~RCControl() {
    // Any cleanup if needed
}

void RCControl::update() {
    // Each loop, read RC signals
    readRCInputs();
}

bool RCControl::isConnected() const {
    return true;
}

void RCControl::readRCInputs() {
    // Read pulse widths (in microseconds) from each channel
    // e.g., typical RC range ~1000 - 2000 microseconds
    unsigned long throttleVal       = pulseIn(throttlePin, HIGH, 25000);
    unsigned long steeringVal       = pulseIn(steeringPin, HIGH, 25000);
    unsigned long gearVal           = pulseIn(gearPin, HIGH, 25000);
    unsigned long turretRotVal      = pulseIn(turretRotationPin, HIGH, 25000);
    unsigned long turretElevVal     = pulseIn(turretElevationPin, HIGH, 25000);
    unsigned long fireVal           = pulseIn(firePin, HIGH, 25000);

    // Convert pulses to a -100..100 range or 1..5 for gear
    int forwardBackward = map(throttleVal, 1000, 2000, -100, 100);
    int turn            = map(steeringVal, 1000, 2000, -100, 100);

    // Gear
    int gearValue = map(gearVal, 1000, 2000, 1, 5);
    gearValue = constrain(gearValue, 1, 5);
    currentGear = gearValue;

    // Combine forward/back + turn for each track
    float gearScaling = (float)currentGear / 5.0f;
    leftTrackSpeed  = constrain((forwardBackward + turn) * gearScaling, -100, 100);
    rightTrackSpeed = constrain((forwardBackward - turn) * gearScaling, -100, 100);

    // Turret
    turretRotation = map(turretRotVal, 1000, 2000, -100, 100);
    turretElevation= map(turretElevVal, 1000, 2000, -100, 100);

    // Flamethrower if e.g. > 1500 microseconds
    flamethrowerActive = (fireVal > 1500);

    // Debug (optional)
    // Serial.printf("RC: throttle=%lu steer=%lu gear=%lu turretR=%lu turretE=%lu fire=%lu\n",
    //              throttleVal, steeringVal, gearVal, turretRotVal, turretElevVal, fireVal);
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
