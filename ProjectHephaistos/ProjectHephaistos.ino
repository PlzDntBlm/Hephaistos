#include <Arduino.h>
#include "TankControlInterface.h"
#include "SerialControl.h"

// Define control mode
#define CONTROL_MODE_SERIAL
#define PIN 27

// Create a pointer to the control interface
TankControlInterface* tankControl = nullptr;

void setup() {
    Serial.begin(115200);
    pinMode(PIN , OUTPUT);

#ifdef CONTROL_MODE_SERIAL
    tankControl = new SerialControl();
    Serial.println("Control Mode: Serial Control via Laptop Keyboard");
#else
    #error "No control mode defined!"
#endif

    // Additional setup code for your tank hardware
    // Initialize motors, servos, etc.
}

void loop() {
    // Update control inputs
    tankControl->update();

    // Get control values
    int leftSpeed = tankControl->getLeftTrackSpeed();
    int rightSpeed = tankControl->getRightTrackSpeed();
    int turretRot = tankControl->getTurretRotation();
    int turretElev = tankControl->getTurretElevation();
    bool flamethrower = tankControl->isFlamethrowerActive();
    int gear = tankControl->getCurrentGear();

    // Control your tank hardware with these values
    // For example:
    // setMotorSpeed(leftMotorPin, leftSpeed);
    // setMotorSpeed(rightMotorPin, rightSpeed);
    // setServoPosition(turretRotationServoPin, turretRot);
    // setServoPosition(turretElevationServoPin, turretElev);
    // controlFlamethrower(flamethrower);

    // For testing purposes, print the values
    Serial.println("------ Tank Control Status ------");
    Serial.printf("Gear: %d\n", gear);
    Serial.printf("Left Track Speed: %d\n", leftSpeed);
    Serial.printf("Right Track Speed: %d\n", rightSpeed);
    Serial.printf("Turret Rotation: %d\n", turretRot);
    Serial.printf("Turret Elevation: %d\n", turretElev);
    Serial.printf("Flamethrower Active: %s\n", flamethrower ? "Yes" : "No");
    if (flamethrower == true) {
      digitalWrite(PIN, HIGH);
    } else {
      digitalWrite(PIN, LOW);
    }


    Serial.println("---------------------------------");

    // Delay to prevent flooding the serial output
    delay(100);
}
