# pip install pyserial pynput

import serial
import threading
from pynput import keyboard

# Update this with the correct serial port (e.g., COM5 on Windows, /dev/ttyUSB0 on Linux)
SERIAL_PORT = 'COM5'  # Replace with your port
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")
except serial.SerialException as e:
    print(f"Error opening serial port {SERIAL_PORT}: {e}")
    exit(1)

# Mapping keys to commands
key_command_map = {
    'w': 'forward',
    'a': 'left',
    's': 'back',
    'd': 'right',
    'q': 'turret_left',
    'e': 'turret_right',
    'space': 'fire',
}

# Set to keep track of pressed keys
pressed_keys = set()

def on_press(key):
    try:
        char = key.char.lower()
        if char in key_command_map:
            command = key_command_map[char] + '_press'
            ser.write((command + '\n').encode())
            pressed_keys.add(char)
    except AttributeError:
        # Handle special keys
        if key == keyboard.Key.up:
            ser.write(('turret_elevate_press\n').encode())
            pressed_keys.add('up')
        elif key == keyboard.Key.down:
            ser.write(('turret_lower_press\n').encode())
            pressed_keys.add('down')
        elif key == keyboard.Key.space:
            ser.write(('fire_press\n').encode())
            pressed_keys.add('space')
        elif key == keyboard.Key.shift:
            ser.write(('gear_up\n').encode())
        elif key == keyboard.Key.ctrl_l or key == keyboard.Key.ctrl_r:
            ser.write(('gear_down\n').encode())

def on_release(key):
    try:
        char = key.char.lower()
        if char in key_command_map:
            command = key_command_map[char] + '_release'
            ser.write((command + '\n').encode())
            pressed_keys.discard(char)
    except AttributeError:
        # Handle special keys
        if key == keyboard.Key.up:
            ser.write(('turret_elevate_release\n').encode())
            pressed_keys.discard('up')
        elif key == keyboard.Key.down:
            ser.write(('turret_lower_release\n').encode())
            pressed_keys.discard('down')
        elif key == keyboard.Key.space:
            ser.write(('fire_release\n').encode())
            pressed_keys.discard('space')
        elif key == keyboard.Key.esc:
            # Stop listener
            ser.close()
            return False

def keyboard_listener():
    with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
        listener.join()

def serial_reader():
    while True:
        try:
            line = ser.readline().decode('utf-8', errors='ignore').rstrip()
            if line:
                print(f"ESP32: {line}")
        except serial.SerialException as e:
            print(f"Serial error: {e}")
            break

# Start the keyboard listener in a separate thread
keyboard_thread = threading.Thread(target=keyboard_listener)
keyboard_thread.daemon = True
keyboard_thread.start()

# Read from serial in the main thread
serial_reader()