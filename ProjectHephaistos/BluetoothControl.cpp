#include "BluetoothControl.h"

// Static members must be defined outside the class
BluetoothControl* BluetoothControl::instance = nullptr;
ControllerPtr BluetoothControl::controller = nullptr;

BluetoothControl::BluetoothControl()
    : currentGear(1),
      leftTrackSpeed(0),
      rightTrackSpeed(0),
      turretRotation(0),
      turretElevation(0),
      flamethrowerActive(false),
      forwardPressed(false),
      backPressed(false),
      leftPressed(false),
      rightPressed(false),
      turretLeftPressed(false),
      turretRightPressed(false),
      turretElevatePressed(false),
      turretLowerPressed(false),
      firePressed(false),
      lastUpdateTime(0)
{
    instance = this;

    // Initialize Bluepad32
    BP32.setup(&BluetoothControl::onConnectedController, &BluetoothControl::onDisconnectedController);
    BP32.enableNewBluetoothConnections(true);

    Serial.println("BluetoothControl Initialized. Waiting for gamepad...");
}

BluetoothControl::~BluetoothControl() {
    // Optionally forget Bluetooth keys if you want to force re-pairing
    // BP32.forgetBluetoothKeys();
}

void BluetoothControl::update() {
    // Fetch controller data
    BP32.update();

    // If there's a connected controller, process it
    if (controller && controller->isConnected()) {
        processGamepad(controller);
    }
}

bool BluetoothControl::isConnected() const {
    return (controller && controller->isConnected());
}

int BluetoothControl::getLeftTrackSpeed() const {
    return leftTrackSpeed;
}

int BluetoothControl::getRightTrackSpeed() const {
    return rightTrackSpeed;
}

int BluetoothControl::getTurretRotation() const {
    return turretRotation;
}

int BluetoothControl::getTurretElevation() const {
    return turretElevation;
}

bool BluetoothControl::isFlamethrowerActive() const {
    return flamethrowerActive;
}

int BluetoothControl::getCurrentGear() const {
    return currentGear;
}

// ----------------------
// Static Callbacks
// ----------------------
void BluetoothControl::onConnectedController(ControllerPtr ctl) {
    Serial.println("Gamepad connected via Bluetooth.");
    controller = ctl;
}

void BluetoothControl::onDisconnectedController(ControllerPtr ctl) {
    Serial.println("Gamepad disconnected.");
    if (ctl == controller) {
        controller = nullptr;
    }
}

// ----------------------
// Process Gamepad
// ----------------------
void BluetoothControl::processGamepad(ControllerPtr ctl) {
    // Limit update rate
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime < updateInterval) {
        return;
    }
    lastUpdateTime = currentTime;

    // Reset the pressed states
    forwardPressed       = false;
    backPressed          = false;
    leftPressed          = false;
    rightPressed         = false;
    turretLeftPressed    = false;
    turretRightPressed   = false;
    turretElevatePressed = false;
    turretLowerPressed   = false;
    firePressed          = false;

    // Example usage: interpret left joystick Y for forward/back
    // (on many controllers, up is negative Y, so adjust logic as needed)
    int16_t axisY = ctl->axisY(); // -512 up, +511 down
    if (axisY < -100) { 
        forwardPressed = true; 
    } else if (axisY > 100) { 
        backPressed = true; 
    }

    // Interpret left joystick X for turning
    int16_t axisX = ctl->axisX(); // -512 left, +511 right
    if (axisX < -100) {
        leftPressed = true;
    } else if (axisX > 100) {
        rightPressed = true;
    }

    // For turret: use right joystick or certain buttons
    int16_t axisRX = ctl->axisRX();  // -512 left, +511 right
    int16_t axisRY = ctl->axisRY();  // -512 up, +511 down

    if (axisRX < -100) {
        turretLeftPressed = true;
    } else if (axisRX > 100) {
        turretRightPressed = true;
    }

    if (axisRY < -100) {
        turretElevatePressed = true;
    } else if (axisRY > 100) {
        turretLowerPressed = true;
    }

    // Check a button for flamethrower
    // For example, "A" button on many controllers
    if (ctl->a()) {
        firePressed = true;
    }

    // Gear shifting with D-Pad up/down
    uint8_t dpadVal = ctl->dpad();
    // dpad() returns 0x00 to 0x08 or 0x0F depending on library
    // We'll do a simple check if dpadVal == DPAD_UP or DPAD_DOWN
    if (dpadVal == DPAD_UP && currentGear < 5) {
        currentGear++;
        Serial.printf("Gear shifted up to %d\n", currentGear);
    } else if (dpadVal == DPAD_DOWN && currentGear > 1) {
        currentGear--;
        Serial.printf("Gear shifted down to %d\n", currentGear);
    }

    // Now we update the control variables
    updateControlVariables();
}

// ----------------------
// updateControlVariables
// ----------------------
void BluetoothControl::updateControlVariables() {
    // Movement
    int forwardBackward = 0;
    int turn = 0;

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
    float gearScaling = (float)currentGear / 5.0f;

    leftTrackSpeed  = constrain((forwardBackward + turn) * gearScaling, -100, 100);
    rightTrackSpeed = constrain((forwardBackward - turn) * gearScaling, -100, 100);

    // Turret
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

    // Flamethrower
    flamethrowerActive = firePressed;
}
