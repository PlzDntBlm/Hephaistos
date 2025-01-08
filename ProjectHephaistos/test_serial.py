# test_serial.py
import serial

SERIAL_PORT = 'COM5'  # Adjust to your port
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Opened {SERIAL_PORT} successfully!")
    ser.write(b'Hello from Python!\n')
    ser.close()
except serial.SerialException as e:
    print(f"Failed to open {SERIAL_PORT}: {e}")
