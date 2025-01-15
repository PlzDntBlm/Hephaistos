#include <Arduino.h>
#include "TankControlInterface.h"
#include "SerialControl.h"
#include "BluetoothControl.h"
#include "KeyboardControl.h"
#include "RCControl.h"
// or whichever control class you want to use


// Define control mode
// #define CONTROL_MODE_SERIAL
#define CONTROL_MODE_BLUETOOTH
// #define CONTROL_MODE_KEYBOARD
//#define CONTROL_MODE_RC

// -------------------------------
// Relay Pin Definitions
// Choose pins that are safe for GPIO use on your ESP32 module
// (e.g., GPIO4 and GPIO13, or whatever you prefer)
#define LEFT_RELAY_PIN  13    // For left track relay
#define RIGHT_RELAY_PIN 4   // For right track relay
// -------------------------------

// Create a pointer to the control interface
TankControlInterface* tankControl = nullptr;

void setup() {
    Serial.begin(115200);

    // Initialize relay pins
    pinMode(LEFT_RELAY_PIN, OUTPUT);
    pinMode(RIGHT_RELAY_PIN, OUTPUT);

#ifdef CONTROL_MODE_BLUETOOTH
    tankControl = new BluetoothControl();
    Serial.println("Control Mode: Bluetooth Gamepad");
#elif defined(CONTROL_MODE_SERIAL)
    tankControl = new SerialControl();
    Serial.println("Control Mode: Serial Control via Connected Laptop Keyboard");
#elif defined(CONTROL_MODE_KEYBOARD)
    tankControl = new KeyboardControl();
    Serial.println("Control Mode: Keyboard Control via Bluetooth Keyboard");
#elif defined(CONTROL_MODE_RC)
    tankControl = new RCControl();
    Serial.println("Control Mode: RC Control via RC Controller");
#else
    #error "No control mode defined!"
#endif

    // Additional setup code for your tank hardware
    // e.g., Initialize motors, servos, sensors, etc.
}

void loop() {
    // Update control inputs (keyboard commands, joystick data, etc.)
    tankControl->update();

    // If there's no device connected, skip printing
    if (!tankControl->isConnected()) {
        delay(100);
        return;
    }

    // Get control values
    int leftSpeed   = tankControl->getLeftTrackSpeed();
    int rightSpeed  = tankControl->getRightTrackSpeed();
    int turretRot   = tankControl->getTurretRotation();
    int turretElev  = tankControl->getTurretElevation();
    bool flamethrower = tankControl->isFlamethrowerActive();
    int gear        = tankControl->getCurrentGear();

    // -------------------------------
    // SMART RELAY CONTROL
    //
    // If speed is non-zero, turn relay ON (HIGH).
    // If speed is zero, turn relay OFF (LOW).
    // You can extend or refine this logic if you want
    // to differentiate between forward/reverse, etc.
    // -------------------------------
    if (leftSpeed > 0) {
      digitalWrite(LEFT_RELAY_PIN, HIGH);
    } else {
      digitalWrite(LEFT_RELAY_PIN, LOW);
    }

    if (rightSpeed > 0) {
      digitalWrite(RIGHT_RELAY_PIN, HIGH);
    } else {
      digitalWrite(RIGHT_RELAY_PIN, LOW);
    }

    bool noMovement = (leftSpeed == 0 && rightSpeed == 0);
    bool noTurret   = (tankControl->getTurretRotation() == 0 && tankControl->getTurretElevation() == 0);
    bool noFlame    = !tankControl->isFlamethrowerActive();

    if (noMovement && noTurret && noFlame) {
        // No reason to print anything
        delay(100);
        return;
    }

    // Control your other tank hardware (motors, servos, etc.)
    // setMotorSpeed(leftMotorPin, leftSpeed);
    // setMotorSpeed(rightMotorPin, rightSpeed);
    // setServoPosition(turretRotationServoPin, turretRot);
    // setServoPosition(turretElevationServoPin, turretElev);
    // controlFlamethrower(flamethrower);

    // For testing/logging purposes, print the values
    Serial.println("------ Tank Control Status ------");
    Serial.printf("Gear: %d\n", gear);
    Serial.printf("Left Track Speed: %d\n", leftSpeed);
    Serial.printf("Right Track Speed: %d\n", rightSpeed);
    Serial.printf("Turret Rotation: %d\n", turretRot);
    Serial.printf("Turret Elevation: %d\n", turretElev);
    Serial.printf("Flamethrower Active: %s\n", flamethrower ? "Yes" : "No");
    Serial.println("---------------------------------");

    // Delay to prevent flooding the serial output
    delay(100);
}
