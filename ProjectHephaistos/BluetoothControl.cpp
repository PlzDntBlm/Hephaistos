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
      // NEW: Initialize gear-shift states for edge detection
      dpadUpPrevious(false),
      dpadDownPrevious(false),
      lastUpdateTime(0)
{
    instance = this;

    // Initialize Bluepad32
    BP32.setup(&BluetoothControl::onConnectedController, &BluetoothControl::onDisconnectedController);
    BP32.enableNewBluetoothConnections(true);

    Serial.println("BluetoothControl Initialized. Waiting for gamepad...");
}

BluetoothControl::~BluetoothControl() {
    // Optionally forget Bluetooth keys if needed:
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

    // Reset the pressed states each cycle
    forwardPressed       = false;
    backPressed          = false;
    leftPressed          = false;
    rightPressed         = false;
    turretLeftPressed    = false;
    turretRightPressed   = false;
    turretElevatePressed = false;
    turretLowerPressed   = false;
    firePressed          = false;

    // -----------------------------
    // 1) Read and apply deadzone
    // -----------------------------
    int16_t axisY = ctl->axisY();  // range ~ -512..+511
    int16_t axisX = ctl->axisX();

    if (abs(axisY) < JOYSTICK_DEADZONE) {
        axisY = 0;
    }
    if (abs(axisX) < JOYSTICK_DEADZONE) {
        axisX = 0;
    }

    // If axisY < 0 => forward, axisY > 0 => back
    if (axisY < 0) {
        forwardPressed = true;
    } else if (axisY > 0) {
        backPressed = true;
    }

    // If axisX < 0 => left, axisX > 0 => right
    if (axisX < 0) {
        leftPressed = true;
    } else if (axisX > 0) {
        rightPressed = true;
    }

    // For turret, let's read the right joystick as well
    int16_t axisRX = ctl->axisRX(); 
    int16_t axisRY = ctl->axisRY();

    if (abs(axisRX) < JOYSTICK_DEADZONE) {
        axisRX = 0;
    }
    if (abs(axisRY) < JOYSTICK_DEADZONE) {
        axisRY = 0;
    }

    if (axisRX < 0) {
        turretLeftPressed = true;
    } else if (axisRX > 0) {
        turretRightPressed = true;
    }

    if (axisRY < 0) {
        turretElevatePressed = true;
    } else if (axisRY > 0) {
        turretLowerPressed = true;
    }

    // Example: "A" button for flamethrower
    if (ctl->a()) {
        firePressed = true;
    }

    // -----------------------------
    // 2) Edge detection for gear shift
    // -----------------------------
    uint8_t dpadVal = ctl->dpad();
    bool dpadUpCurrent   = (dpadVal == DPAD_UP);
    bool dpadDownCurrent = (dpadVal == DPAD_DOWN);

    // If we see an UP press that wasn't pressed last time, gear up
    if (dpadUpCurrent && !dpadUpPrevious && currentGear < 5) {
        currentGear++;
        Serial.printf("Gear shifted up to %d\n", currentGear);
    }
    // If we see a DOWN press that wasn't pressed last time, gear down
    if (dpadDownCurrent && !dpadDownPrevious && currentGear > 1) {
        currentGear--;
        Serial.printf("Gear shifted down to %d\n", currentGear);
    }

    // Update our "previous" states
    dpadUpPrevious   = dpadUpCurrent;
    dpadDownPrevious = dpadDownCurrent;

    // Now we update the final control variables
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