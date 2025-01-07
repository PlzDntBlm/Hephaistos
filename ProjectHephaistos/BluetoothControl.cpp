#include "BluetoothControl.h"

// Initialize static instance pointer
BluetoothControl* BluetoothControl::instance = nullptr;

// DPAD constants
#define DPAD_CENTERED    0x00
#define DPAD_UP          0x01
#define DPAD_UP_RIGHT    0x02
#define DPAD_RIGHT       0x03
#define DPAD_DOWN_RIGHT  0x04
#define DPAD_DOWN        0x05
#define DPAD_DOWN_LEFT   0x06
#define DPAD_LEFT        0x07
#define DPAD_UP_LEFT     0x08

BluetoothControl::BluetoothControl()
    : controller(nullptr),
      currentGear(1),
      leftTrackSpeed(0),
      rightTrackSpeed(0),
      turretRotation(0),
      turretElevation(0),
      flamethrowerActive(false) {
    instance = this;

    // Initialize Bluepad32
    BP32.setup(&BluetoothControl::onConnectedController, &BluetoothControl::onDisconnectedController);

    // Enable new Bluetooth connections
    BP32.enableNewBluetoothConnections(true);
}

BluetoothControl::~BluetoothControl() {
    BP32.forgetBluetoothKeys();
}

void BluetoothControl::update() {
    // Fetch controller data
    BP32.update();

    if (controller && controller->isConnected()) {
        processGamepad(controller);
    }
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

// Static callback functions
void BluetoothControl::onConnectedController(ControllerPtr ctl) {
    Serial.println("Controller connected via Bluetooth.");
    instance->controller = ctl;
}

void BluetoothControl::onDisconnectedController(ControllerPtr ctl) {
    Serial.println("Controller disconnected from Bluetooth.");
    instance->controller = nullptr;
}

void BluetoothControl::processGamepad(ControllerPtr ctl) {
    // Read the D-Pad value
    uint8_t dpad = ctl->dpad();

    // Update gear based on D-Pad inputs
    if ((dpad == DPAD_UP || dpad == DPAD_UP_LEFT || dpad == DPAD_UP_RIGHT) && currentGear < 5) {
        currentGear++;
    } else if ((dpad == DPAD_DOWN || dpad == DPAD_DOWN_LEFT || dpad == DPAD_DOWN_RIGHT) && currentGear > 1) {
        currentGear--;
    }

    // Read joystick inputs
    int16_t leftJoystickY = ctl->axisY();
    int16_t leftJoystickX = ctl->axisX();
    int16_t rightJoystickX = ctl->axisRX();
    int16_t rightJoystickY = ctl->axisRY();

    // Map joystick values
    int forwardBackward = map(leftJoystickY, -512, 511, 100, -100);
    int turn = map(leftJoystickX, -512, 511, -100, 100);

    float gearScaling = (float)currentGear / 5;

    leftTrackSpeed = constrain((forwardBackward + turn) * gearScaling, -100, 100);
    rightTrackSpeed = constrain((forwardBackward - turn) * gearScaling, -100, 100);

    turretRotation = map(rightJoystickX, -512, 511, -100, 100);
    turretElevation = map(rightJoystickY, -512, 511, 100, -100);

    // Flamethrower activation
    int16_t throttle = ctl->throttle();
    flamethrowerActive = (throttle > 100);
}
