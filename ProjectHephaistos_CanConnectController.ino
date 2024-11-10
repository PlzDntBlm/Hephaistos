#include <Bluepad32.h>

// D-Pad direction constants
#define DPAD_CENTERED    0x00
#define DPAD_UP          0x01
#define DPAD_UP_RIGHT    0x02
#define DPAD_RIGHT       0x03
#define DPAD_DOWN_RIGHT  0x04
#define DPAD_DOWN        0x05
#define DPAD_DOWN_LEFT   0x06
#define DPAD_LEFT        0x07
#define DPAD_UP_LEFT     0x08

// Maximum number of controllers supported by Bluepad32
ControllerPtr myControllers[BP32_MAX_CONTROLLERS];

// Variables for tank control
int currentGear = 1; // Gear ranges from 1 (slowest) to 5 (fastest)
const int maxGear = 5;
const int minGear = 1;

// Variables to hold motor speeds for left and right tracks
int leftTrackSpeed = 0;  // Range: -100 (full reverse) to 100 (full forward)
int rightTrackSpeed = 0; // Range: -100 (full reverse) to 100 (full forward)

// Variables for turret control
int turretRotation = 0;  // Range: -100 (rotate left) to 100 (rotate right)
int turretElevation = 0; // Range: -100 (down) to 100 (up)

// Flamethrower trigger
bool flamethrowerActive = false;

// Callback when a new controller is connected
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller connected, index=%d\n", i);
            // You can get certain controller properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n",
                          ctl->getModelName().c_str(),
                          properties.vendor_id,
                          properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but no empty slot found");
    }
}

// Callback when a controller is disconnected
void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

// Function to process gamepad inputs and map them to tank controls
void processGamepad(ControllerPtr ctl) {
    // Check if any new data is available
    if (!ctl->isConnected() || !ctl->hasData()) {
        return;
    }

    // Read the D-Pad value
    uint8_t dpad = ctl->dpad();

    // Update gear based on D-Pad inputs
    // D-Pad Up: Increase gear
    // D-Pad Down: Decrease gear
    if ((dpad == DPAD_UP || dpad == DPAD_UP_LEFT || dpad == DPAD_UP_RIGHT) && currentGear < maxGear) {
        currentGear++;
        Serial.printf("Gear shifted up to %d\n", currentGear);
    } else if ((dpad == DPAD_DOWN || dpad == DPAD_DOWN_LEFT || dpad == DPAD_DOWN_RIGHT) && currentGear > minGear) {
        currentGear--;
        Serial.printf("Gear shifted down to %d\n", currentGear);
    }

    // Read left joystick inputs for tank movement
    int16_t leftJoystickY = ctl->axisY(); // Forward/backward movement
    int16_t leftJoystickX = ctl->axisX(); // Turning left/right

    // Map joystick values from (-512 to 511) to (-100 to 100)
    // Invert Y-axis for natural forward/backward control
    int forwardBackward = map(leftJoystickY, -512, 511, 100, -100); // Forward is positive
    int turn = map(leftJoystickX, -512, 511, -100, 100); // Left is negative, right is positive

    // Calculate track speeds based on joystick inputs and current gear
    // Apply gear scaling to limit maximum speed
    float gearScaling = (float)currentGear / maxGear;

    // Tank steering logic:
    // Left track speed = forwardBackward + turn
    // Right track speed = forwardBackward - turn
    leftTrackSpeed = constrain((forwardBackward + turn) * gearScaling, -100, 100);
    rightTrackSpeed = constrain((forwardBackward - turn) * gearScaling, -100, 100);

    // Read right joystick inputs for turret control
    int16_t rightJoystickX = ctl->axisRX(); // Turret rotation
    int16_t rightJoystickY = ctl->axisRY(); // Turret elevation

    // Map joystick values to (-100 to 100)
    turretRotation = map(rightJoystickX, -512, 511, -100, 100); // Left/right rotation
    turretElevation = map(rightJoystickY, -512, 511, 100, -100); // Up/down elevation (inverted Y-axis)

    // Read trigger for flamethrower activation
    int16_t throttle = ctl->throttle(); // R2 trigger value (0 to 1023)
    flamethrowerActive = (throttle > 100); // Activate if trigger pressed beyond threshold

    // Output the interpreted actions to the Serial Monitor
    Serial.println("------ Tank Control Status ------");
    Serial.printf("Gear: %d\n", currentGear);
    Serial.printf("Left Track Speed: %d\n", leftTrackSpeed);
    Serial.printf("Right Track Speed: %d\n", rightTrackSpeed);
    Serial.printf("Turret Rotation: %d\n", turretRotation);
    Serial.printf("Turret Elevation: %d\n", turretElevation);
    Serial.printf("Flamethrower Active: %s\n", flamethrowerActive ? "Yes" : "No");
    Serial.println("---------------------------------");
}

// Function to process all connected controllers
void processControllers() {
    // Iterate through all connected controllers
    for (auto ctl : myControllers) {
        if (ctl && ctl->isConnected()) {
            if (ctl->isGamepad()) {
                processGamepad(ctl);
            } else {
                Serial.println("Unsupported controller type connected.");
            }
        }
    }
}

// Arduino setup function. Runs on CPU core 1
void setup() {
    Serial.begin(115200);
    Serial.println("Starting Bluetooth...");

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // Enable new Bluetooth connections
    BP32.enableNewBluetoothConnections(true);
}

// Arduino loop function. Runs on CPU core 1
void loop() {
    // This call fetches all the controllers' data.
    // Call this function in your main loop.
    BP32.update();

    // Process the controllers
    processControllers();

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // Adding a small delay to prevent the watchdog timer from triggering.
    delay(10); // Adjust the delay as necessary
}
