# ESP32-Powered AR Smart Glasses

A reference prototype implementation of an ESP32-powered Augmented Reality (AR) Heads-Up Display (HUD) wearable.

> **Important:** The source code in this repository is a reconstructed reference implementation created from the documented project architecture. It is **not the original source code** used in the original prototype.

## Overview

The system uses an ESP32-WROOM-32 as the embedded controller. A 1.3-inch SH1106 OLED generates the visual HUD, while an angled transparent reflector can be used to create a Pepper's-Ghost-style optical illusion.

The reference implementation also demonstrates:

- Local Wi-Fi access point
- HTTP web-server control
- Dynamic HUD text
- MPU6050 motion sensing
- ESP32 capacitive touch input
- OLED orientation/mirroring commands

## Hardware

- ESP32-WROOM-32
- 1.3-inch SH1106 OLED
- MPU6050
- Capacitive touch input
- AR/transparent reflector
- USB/battery power source

## Pin Configuration

| Component | ESP32 |
|---|---|
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| MPU6050 SDA | GPIO 21 |
| MPU6050 SCL | GPIO 22 |
| Touch input | GPIO 4 |

OLED and MPU6050 share the same I2C bus.

## Arduino Libraries

Install these libraries using Arduino IDE Library Manager:

- Adafruit GFX Library
- Adafruit SH110X
- Adafruit MPU6050
- Adafruit Unified Sensor

The ESP32 board package provides the Wi-Fi and WebServer functionality.

## Wi-Fi

The ESP32 creates a local access point:

- SSID: `AR-Glasses`
- Password: `ARglasses123`

**Change the password before using this on a real/public deployment.**

After connecting to the ESP32 network, open its AP IP address shown in the Serial Monitor.

## HTTP Endpoints

### Display a message

```text
/display?msg=HELLO
```

Example:

```text
http://192.168.4.1/display?msg=CALL
```

### Wake display

```text
/wake
```

### Sleep display

```text
/sleep
```

## Interaction

### Capacitive touch

- Double tap → wake the display
- Long press → sleep the display

The touch threshold is adaptive and may need tuning for the physical electrode.

### Motion

The MPU6050 is used for a prototype gesture detector. The acceleration threshold should be calibrated for the final mechanical design and intended head movements.

## OLED Optical Orientation

The code uses SH1106 segment-remap and COM-scan commands to change the display orientation for an optical reflector arrangement.

The exact orientation should be tested with the physical lens/reflector geometry.

## How to Upload

1. Open `AR_Smart_Glasses.ino` in Arduino IDE.
2. Select an ESP32-WROOM-32 compatible board.
3. Install the required libraries.
4. Connect the ESP32.
5. Select the correct COM port.
6. Upload the sketch.
7. Open Serial Monitor at 115200 baud.
8. Connect a phone to the `AR-Glasses` Wi-Fi network.
9. Test the HTTP endpoints.

## Project Structure

```text
ESP32-AR-Smart-Glasses/
├── AR_Smart_Glasses.ino
├── README.md
└── hardware/
    └── wiring.md
```

## Future Improvements

- Android notification forwarding
- Navigation information
- Voice input/output
- Improved gesture classification
- Battery monitoring
- Custom PCB
- Compact optical enclosure
- Secure authenticated API
