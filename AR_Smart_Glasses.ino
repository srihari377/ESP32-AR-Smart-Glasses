/*
  ESP32 AR Smart Glasses - Reference Prototype
  ------------------------------------------------
  Reconstructed reference implementation based on the documented
  project architecture. This is not the original source code.

  Hardware:
  - ESP32-WROOM-32
  - 1.3" SH1106 OLED (I2C)
  - MPU6050 (I2C)
  - ESP32 capacitive touch input on GPIO 4

  Libraries:
  - Adafruit GFX Library
  - Adafruit SH110X
  - Adafruit MPU6050
  - Adafruit Unified Sensor
*/

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define OLED_SDA 21
#define OLED_SCL 22
#define TOUCH_PIN 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

const char* AP_SSID = "AR-Glasses";
const char* AP_PASSWORD = "ARglasses123";

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MPU6050 mpu;
WebServer server(80);

String hudMessage = "READY";
bool displayAwake = true;

unsigned long lastTouchTime = 0;
unsigned long touchStart = 0;
bool touchWasActive = false;
int touchCount = 0;

const unsigned long DOUBLE_TAP_WINDOW = 500;
const unsigned long LONG_PRESS_TIME = 1000;
const float GESTURE_THRESHOLD = 2.5;

void mirrorOLED(bool enable) {
  // SH1106 segment remap + COM scan direction.
  // Used here to create the orientation needed by a simple
  // Pepper's-Ghost style optical reflector.
  display.oled_command(enable ? 0xA1 : 0xA0);
  display.oled_command(enable ? 0xC8 : 0xC0);
}

void drawHUD() {
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("AR SMART GLASSES");
  display.drawLine(0, 11, 127, 11, SH110X_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 22);
  display.println(hudMessage);

  display.setTextSize(1);
  display.setCursor(0, 53);
  display.print("Touch: ");
  display.print(displayAwake ? "ON" : "OFF");

  display.display();
}

void setMessage(String message) {
  hudMessage = message;
  if (displayAwake) {
    drawHUD();
  }
}

void handleRoot() {
  String html =
    "<html><body>"
    "<h2>ESP32 AR Smart Glasses</h2>"
    "<p>Use /display?msg=HELLO</p>"
    "<p>Use /wake or /sleep</p>"
    "</body></html>";
  server.send(200, "text/html", html);
}

void handleDisplay() {
  if (!server.hasArg("msg")) {
    server.send(400, "text/plain", "Missing msg");
    return;
  }

  String message = server.arg("msg");
  message.replace("+", " ");
  setMessage(message);
  server.send(200, "text/plain", "Displayed: " + message);
}

void handleWake() {
  displayAwake = true;
  setMessage("AWAKE");
  server.send(200, "text/plain", "Display awake");
}

void handleSleep() {
  displayAwake = false;
  display.clearDisplay();
  display.display();
  server.send(200, "text/plain", "Display sleeping");
}

void detectTouch() {
  // ESP32 touchRead() returns a lower value when the pad is touched.
  // Threshold may need calibration for the physical build.
  static int baseline = 0;
  int value = touchRead(TOUCH_PIN);

  if (baseline == 0) baseline = value;
  baseline = (baseline * 9 + value) / 10;

  bool active = value < (baseline * 0.65);

  if (active && !touchWasActive) {
    touchStart = millis();

    if (millis() - lastTouchTime < DOUBLE_TAP_WINDOW) {
      touchCount++;
    } else {
      touchCount = 1;
    }
    lastTouchTime = millis();
  }

  if (!active && touchWasActive) {
    unsigned long duration = millis() - touchStart;

    if (duration >= LONG_PRESS_TIME) {
      displayAwake = false;
      display.clearDisplay();
      display.display();
      touchCount = 0;
    } else if (touchCount >= 2) {
      displayAwake = true;
      setMessage("WAKE");
      touchCount = 0;
    }
  }

  if (touchCount == 1 && millis() - lastTouchTime > DOUBLE_TAP_WINDOW) {
    touchCount = 0;
  }

  touchWasActive = active;
}

void detectGesture() {
  static unsigned long lastGesture = 0;

  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  if (millis() - lastGesture < 1000) return;

  float magnitude = sqrt(
    accel.acceleration.x * accel.acceleration.x +
    accel.acceleration.y * accel.acceleration.y +
    accel.acceleration.z * accel.acceleration.z
  );

  // Simple prototype gesture: strong acceleration/deviation.
  // Tune threshold and axis logic for the final mechanical design.
  if (fabs(magnitude - 9.81) > GESTURE_THRESHOLD) {
    setMessage("GESTURE");
    lastGesture = millis();
  }
}

void setup() {
  Serial.begin(115200);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(0x3C, true)) {
    Serial.println("SH1106 OLED not found.");
    while (true) delay(100);
  }

  display.clearDisplay();
  display.display();

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found.");
  } else {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  }

  // Optional orientation for the optical reflector.
  mirrorOLED(true);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/display", handleDisplay);
  server.on("/wake", handleWake);
  server.on("/sleep", handleSleep);
  server.begin();

  setMessage("READY");
}

void loop() {
  server.handleClient();
  detectTouch();
  detectGesture();
  delay(10);
}
