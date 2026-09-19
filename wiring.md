# Wiring Reference

## OLED SH1106

| OLED | ESP32 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

## MPU6050

| MPU6050 | ESP32 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

The OLED and MPU6050 share the ESP32 I2C bus.

## Capacitive Touch

Use an ESP32 touch-capable GPIO. The reference code uses:

```text
Touch electrode → GPIO 4
```

The physical touch threshold must be calibrated after assembly.
