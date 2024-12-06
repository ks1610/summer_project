#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// Create objects for the sensors
Adafruit_VL53L0X lox[2] = {Adafruit_VL53L0X(), Adafruit_VL53L0X()};

// XSHUT pin definitions
#define XSHUT1 8
#define XSHUT2 9
const uint8_t XSHUT_PINS[2] = {XSHUT1, XSHUT2};

// Ultrasonic sensor pin definitions
unsigned int TRIG_PIN[2] = {2, 4};
unsigned int ECHO_PIN[2] = {3, 5};

// Constants
const unsigned int BAUD_RATE = 9600;

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

// Function to read distance from ultrasonic sensors
void readDistanceUltrasonic(unsigned int trigPin, unsigned int echoPin, int sensorIndex) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH);
  int distance = duration / 29 / 2; // Convert to distance in cm

  if (duration == 0) {
    Serial.print("Warning: no pulse from ultrasonic sensor ");
    Serial.println(sensorIndex);
  } else {
    Serial.print("Ultrasonic Sensor ");
    Serial.print(sensorIndex);
    Serial.print(" Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
}

void setup() {
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
  // Read and print distances from VL53L0X sensors
  readDistanceVL53L0X(lox[0], "VL53L0X Sensor 1");
  readDistanceVL53L0X(lox[1], "VL53L0X Sensor 2");

  // Read and print distances from ultrasonic sensors
  for (int i = 0; i < 2; i++) {
    readDistanceUltrasonic(TRIG_PIN[i], ECHO_PIN[i], i);
  }

  delay(1000); // Delay between readings
}
