#include "KeyboardControl.h"

// Static members
KeyboardControl* KeyboardControl::instance = nullptr;
ControllerPtr KeyboardControl::keyboardController = nullptr;

KeyboardControl::KeyboardControl()
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

    // Initialize Bluepad32 for keyboard
    BP32.setup(&KeyboardControl::onConnectedController, &KeyboardControl::onDisconnectedController);
    BP32.enableNewBluetoothConnections(true);

    Serial.println("KeyboardControl Initialized. Waiting for a BT Keyboard...");
}

KeyboardControl::~KeyboardControl() {
    // Optionally forget keys or do other cleanup
    // BP32.forgetBluetoothKeys();
}

void KeyboardControl::update() {
    // Poll Bluepad32
    BP32.update();

    // If a keyboard is connected, process it
    if (keyboardController && keyboardController->isConnected()) {
        processKeyboard(keyboardController);
    }
}

bool KeyboardControl::isConnected() const {
    return (keyboardController && keyboardController->isConnected());
}

int KeyboardControl::getLeftTrackSpeed() const {
    return leftTrackSpeed;
}

int KeyboardControl::getRightTrackSpeed() const {
    return rightTrackSpeed;
}

int KeyboardControl::getTurretRotation() const {
    return turretRotation;
}

int KeyboardControl::getTurretElevation() const {
    return turretElevation;
}

bool KeyboardControl::isFlamethrowerActive() const {
    return flamethrowerActive;
}

int KeyboardControl::getCurrentGear() const {
    return currentGear;
}

// Static callbacks
void KeyboardControl::onConnectedController(ControllerPtr ctl) {
    if (ctl->isKeyboard()) {
        Serial.println("Bluetooth Keyboard connected.");
        keyboardController = ctl;
    }
}

void KeyboardControl::onDisconnectedController(ControllerPtr ctl) {
    if (ctl == keyboardController) {
        Serial.println("Bluetooth Keyboard disconnected.");
        keyboardController = nullptr;
    }
}

void KeyboardControl::processKeyboard(ControllerPtr ctl) {
    // Limit update rate
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime < updateInterval) {
        return;
    }
    lastUpdateTime = currentTime;

    // Reset all pressed states
    forwardPressed       = ctl->isKeyPressed(KeyboardKey::Keyboard_W);
    backPressed          = ctl->isKeyPressed(KeyboardKey::Keyboard_S);
    leftPressed          = ctl->isKeyPressed(KeyboardKey::Keyboard_A);
    rightPressed         = ctl->isKeyPressed(KeyboardKey::Keyboard_D);
    turretLeftPressed    = ctl->isKeyPressed(KeyboardKey::Keyboard_Q);
    turretRightPressed   = ctl->isKeyPressed(KeyboardKey::Keyboard_E);
    turretElevatePressed = ctl->isKeyPressed(KeyboardKey::Keyboard_UpArrow);
    turretLowerPressed   = ctl->isKeyPressed(KeyboardKey::Keyboard_DownArrow);
    firePressed          = ctl->isKeyPressed(KeyboardKey::Keyboard_Spacebar);

    // Gear up if Shift pressed, gear down if Ctrl pressed
    bool shiftPressed = ctl->isKeyPressed(KeyboardKey::Keyboard_LeftShift) ||
                        ctl->isKeyPressed(KeyboardKey::Keyboard_RightShift);
    bool ctrlPressed  = ctl->isKeyPressed(KeyboardKey::Keyboard_LeftControl) ||
                        ctl->isKeyPressed(KeyboardKey::Keyboard_RightControl);

    if (shiftPressed && currentGear < 5) {
        currentGear++;
        Serial.printf("Gear shifted up to %d\n", currentGear);
    }
    if (ctrlPressed && currentGear > 1) {
        currentGear--;
        Serial.printf("Gear shifted down to %d\n", currentGear);
    }

    // Now update control variables
    updateControlVariables();
}

void KeyboardControl::updateControlVariables() {
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

    flamethrowerActive = firePressed;
}
