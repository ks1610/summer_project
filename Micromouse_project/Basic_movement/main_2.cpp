#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// Create objects for the sensors
Adafruit_VL53L0X lox[2] = {Adafruit_VL53L0X(), Adafruit_VL53L0X()};

// Font & Back

// XSHUT pin definitions
#define XSHUT1 8
#define XSHUT2 9
const uint8_t XSHUT_PINS[2] = {XSHUT1, XSHUT2};

// Right & Left 

// Ultrasonic sensor pin definitions
unsigned int TRIG_PIN[2] = {2, 4};
unsigned int ECHO_PIN[2] = {3, 5};

// Constants
const unsigned int BAUD_RATE = 9600;

int motor1pin1 = 10;
int motor1pin2 = 11;
int motor2pin1 = 12;
int motor2pin2 = 13;

// Function to initialize a VL53L0X sensor
bool initSensor(Adafruit_VL53L0X& sensor, uint8_t xshutPin, uint8_t address) {
  digitalWrite(xshutPin, LOW);  // Turn off the sensor
  delay(10);
  digitalWrite(xshutPin, HIGH); // Turn on the sensor
  delay(10);
  return sensor.begin(address); // Initialize with a unique I2C address
}

// Function to read distance from a VL53L0X sensor
void readDistanceVL53L0X(Adafruit_VL53L0X& sensor, const char* label) {
  VL53L0X_RangingMeasurementData_t measure;
  sensor.rangingTest(&measure, false);
  Serial.print(label);
  Serial.print(" Distance (mm): ");
  if (measure.RangeStatus != 4) {
    Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println("out of range");
  }
}

// Helper function for ultrasonic sensor readings
int readUltrasonicDistance(int index) {
  digitalWrite(TRIG_PIN[index], LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN[index], HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN[index], LOW);

  unsigned long duration = pulseIn(ECHO_PIN[index], HIGH);
  return duration / 29 / 2; // Convert to distance in cm
}
void Move_forward() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
  delay(300);
}

void Move_backward() {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  delay(300);
}

void Move_left() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  delay(200);
}

void Move_right() {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
  delay(200);
}

void setup() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  //(Optional)
  pinMode(A0, OUTPUT); 
  pinMode(A1, OUTPUT);

  Serial.begin(BAUD_RATE);

  // Initialize ultrasonic sensor pins
  for (int i = 0; i < 2; i++) {
    pinMode(TRIG_PIN[i], OUTPUT);
    pinMode(ECHO_PIN[i], INPUT);
  }

  // Initialize XSHUT pins
  for (int i = 0; i < 2; i++) {
    pinMode(XSHUT_PINS[i], OUTPUT);
    digitalWrite(XSHUT_PINS[i], LOW); // Ensure sensors are off initially
  }

  // Initialize VL53L0X sensors
  if (!initSensor(lox[0], XSHUT1, 0x30)) {
    Serial.println(F("Failed to boot VL53L0X #1"));
    while (1);
  }
  if (!initSensor(lox[1], XSHUT2, 0x31)) {
    Serial.println(F("Failed to boot VL53L0X #2"));
    while (1);
  }
}

void loop() {
  // Read distances from sensors
  VL53L0X_RangingMeasurementData_t measureFront, measureBack;

  lox[0].rangingTest(&measureFront, false); // Front sensor

  int leftDistance = readUltrasonicDistance(0);  // Left ultrasonic sensor
  int rightDistance = readUltrasonicDistance(1); // Right ultrasonic sensor

  // Define thresholds (in mm for VL53L0X, cm for ultrasonic)
  const int frontThreshold = 20;  // 20 mm for front sensor
  const int sideThreshold = 2;    // 2 cm for ultrasonic sides

  // Movement logic
  if (measureFront.RangeStatus != 4 && measureFront.RangeMilliMeter < frontThreshold) {
    // Obstacle in front
    Serial.println("Obstacle detected in front. Deciding turn or move backward.");

    if (leftDistance > sideThreshold) {
      Serial.println("Left is clear. Turning left.");
      Move_left();
    } else if (rightDistance > sideThreshold) {
      Serial.println("Right is clear. Turning right.");
      Move_right();
    } else {
      Serial.println("No clear path. Moving backward.");
      Move_backward();
    }
  } else {
    // Front is clear
    Serial.println("Path is clear. Moving forward.");
    Move_forward();
  }

  // Delay to prevent rapid changes
  delay(100);
}
