#include "SerialControl.h"

SerialControl::SerialControl()
    : currentGear(1),
      leftTrackSpeed(0),
      rightTrackSpeed(0),
      turretRotation(0),
      turretElevation(0),
      flamethrowerActive(false),
      lastUpdateTime(0),
      forwardPressed(false),
      backPressed(false),
      leftPressed(false),
      rightPressed(false),
      turretLeftPressed(false),
      turretRightPressed(false),
      turretElevatePressed(false),
      turretLowerPressed(false),
      firePressed(false) {
    // Initialize Serial communication
    Serial.begin(115200);
    while (!Serial) {
        ; // Wait for Serial port to connect (needed for native USB)
    }
    Serial.println("Serial Control Initialized.");
}

SerialControl::~SerialControl() {
    // Destructor logic if needed
}

void SerialControl::update() {
    // Read any available serial input
    processSerialInput();

    // Limit update rate
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentTime;
        updateControlVariables();
    }
}

bool SerialControl::isConnected() const {
    return true;
}

void SerialControl::processSerialInput() {
    // Read lines from Serial
    while (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (command.length() == 0) {
            continue;
        }
        if (command == "forward_press") {
            forwardPressed = true;
        } else if (command == "forward_release") {
            forwardPressed = false;
        } else if (command == "back_press") {
            backPressed = true;
        } else if (command == "back_release") {
            backPressed = false;
        } else if (command == "left_press") {
            leftPressed = true;
        } else if (command == "left_release") {
            leftPressed = false;
        } else if (command == "right_press") {
            rightPressed = true;
        } else if (command == "right_release") {
            rightPressed = false;
        } else if (command == "turret_left_press") {
            turretLeftPressed = true;
        } else if (command == "turret_left_release") {
            turretLeftPressed = false;
        } else if (command == "turret_right_press") {
            turretRightPressed = true;
        } else if (command == "turret_right_release") {
            turretRightPressed = false;
        } else if (command == "turret_elevate_press") {
            turretElevatePressed = true;
        } else if (command == "turret_elevate_release") {
            turretElevatePressed = false;
        } else if (command == "turret_lower_press") {
            turretLowerPressed = true;
        } else if (command == "turret_lower_release") {
            turretLowerPressed = false;
        } else if (command == "fire_press") {
            firePressed = true;
        } else if (command == "fire_release") {
            firePressed = false;
        } else if (command == "gear_up") {
            if (currentGear < 5) {
                currentGear++;
                Serial.printf("Gear shifted up to %d\n", currentGear);
            }
        } else if (command == "gear_down") {
            if (currentGear > 1) {
                currentGear--;
                Serial.printf("Gear shifted down to %d\n", currentGear);
            }
        } else {
            // Unknown command
            Serial.print("Unknown command: ");
            Serial.println(command);
        }
    }
}

void SerialControl::updateControlVariables() {
    // Reset movement variables
    int forwardBackward = 0;
    int turn = 0;

    // Movement control
    if (forwardPressed) {
        forwardBackward += 100;
    }
    if (backPressed) {
        forwardBackward -= 100;
    }
    if (leftPressed) {
        turn -= 100;
    }
    if (rightPressed) {
        turn += 100;
    }

    // Gear scaling
    float gearScaling = (float)currentGear / 5;

    leftTrackSpeed = constrain((forwardBackward + turn) * gearScaling, -100, 100);
    rightTrackSpeed = constrain((forwardBackward - turn) * gearScaling, -100, 100);

    // Turret control
    if (turretLeftPressed) {
        turretRotation = -100;
    } else if (turretRightPressed) {
        turretRotation = 100;
    } else {
        turretRotation = 0;
    }

    if (turretElevatePressed) {
        turretElevation = 100;
    } else if (turretLowerPressed) {
        turretElevation = -100;
    } else {
        turretElevation = 0;
    }

    // Flamethrower activation
    flamethrowerActive = firePressed;
}

int SerialControl::getLeftTrackSpeed() const {
    return leftTrackSpeed;
}

int SerialControl::getRightTrackSpeed() const {
    return rightTrackSpeed;
}

int SerialControl::getTurretRotation() const {
    return turretRotation;
}

int SerialControl::getTurretElevation() const {
    return turretElevation;
}

bool SerialControl::isFlamethrowerActive() const {
    return flamethrowerActive;
}

int SerialControl::getCurrentGear() const {
    return currentGear;
}
