#include "KeyboardControl.h"

// Initialize static instance pointer
KeyboardControl* KeyboardControl::instance = nullptr;

KeyboardControl::KeyboardControl()
    : keyboardController(nullptr),
      currentGear(1),
      leftTrackSpeed(0),
      rightTrackSpeed(0),
      turretRotation(0),
      turretElevation(0),
      flamethrowerActive(false),
      wPressed(false),
      sPressed(false),
      aPressed(false),
      dPressed(false),
      upPressed(false),
      downPressed(false),
      leftPressed(false),
      rightPressed(false),
      ePressed(false),
      qPressed(false),
      spacePressed(false),
      lastUpdateTime(0) {
    instance = this;

    // Initialize Bluepad32
    BP32.setup(&KeyboardControl::onConnectedController, &KeyboardControl::onDisconnectedController);

    // Enable new Bluetooth connections
    BP32.enableNewBluetoothConnections(true);
}

KeyboardControl::~KeyboardControl() {
    BP32.forgetBluetoothKeys();
}

void KeyboardControl::update() {
    // Fetch controller data
    BP32.update();

    if (keyboardController && keyboardController->isConnected()) {
        processKeyboard(keyboardController);
    }
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

// Static callback functions
void KeyboardControl::onConnectedController(ControllerPtr ctl) {
    if (ctl->isKeyboard()) {
        Serial.println("Keyboard connected via Bluetooth.");
        instance->keyboardController = ctl;
    }
}

void KeyboardControl::onDisconnectedController(ControllerPtr ctl) {
    if (ctl == instance->keyboardController) {
        Serial.println("Keyboard disconnected from Bluetooth.");
        instance->keyboardController = nullptr;
    }
}

void KeyboardControl::processKeyboard(ControllerPtr ctl) {
    // Limit update rate
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime < updateInterval) {
        return;
    }
    lastUpdateTime = currentTime;

    // Reset control variables
    leftTrackSpeed = 0;
    rightTrackSpeed = 0;
    turretRotation = 0;
    turretElevation = 0;
    flamethrowerActive = false;

    // Update key states
    wPressed = ctl->isKeyPressed(KeyboardKey::Keyboard_W);
    sPressed = ctl->isKeyPressed(KeyboardKey::Keyboard_S);
    aPressed = ctl->isKeyPressed(KeyboardKey::Keyboard_A);
    dPressed = ctl->isKeyPressed(KeyboardKey::Keyboard_D);
    upPressed = ctl->isKeyPressed(KeyboardKey::Keyboard_UpArrow);
    downPressed = ctl->isKeyPressed(KeyboardKey::Keyboard_DownArrow);
    leftPressed = ctl->isKeyPressed(KeyboardKey::Keyboard_LeftArrow);
    rightPressed = ctl->isKeyPressed(KeyboardKey::Keyboard_RightArrow);
    ePressed = ctl->isKeyPressed(KeyboardKey::Keyboard_E);
    qPressed = ctl->isKeyPressed(KeyboardKey::Keyboard_Q);
    spacePressed = ctl->isKeyPressed(KeyboardKey::Keyboard_Spacebar);

    // Movement control
    int forwardBackward = 0;
    int turn = 0;

    if (wPressed) {
        forwardBackward += 100;
    }
    if (sPressed) {
        forwardBackward -= 100;
    }
    if (aPressed) {
        turn -= 100;
    }
    if (dPressed) {
        turn += 100;
    }

    // Gear scaling
    float gearScaling = (float)currentGear / 5;

    leftTrackSpeed = constrain((forwardBackward + turn) * gearScaling, -100, 100);
    rightTrackSpeed = constrain((forwardBackward - turn) * gearScaling, -100, 100);

    // Gear shifting
    if (ePressed && currentGear < 5) {
        currentGear++;
    }
    if (qPressed && currentGear > 1) {
        currentGear--;
    }

    // Turret control
    if (leftPressed) {
        turretRotation = -100;
    }
    if (rightPressed) {
        turretRotation = 100;
    }
    if (upPressed) {
        turretElevation = 100;
    }
    if (downPressed) {
        turretElevation = -100;
    }

    // Flamethrower activation
    flamethrowerActive = spacePressed;
}
