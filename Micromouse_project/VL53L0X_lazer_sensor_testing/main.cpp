#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// Create objects for the two sensors
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

#define XSHUT1 7 // XSHUT pin for Sensor 1
#define XSHUT2 8 // XSHUT pin for Sensor 2

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }

  Serial.println("Adafruit VL53L0X Dual Sensor Test");

  // Initialize GPIO pins for XSHUT
  pinMode(XSHUT1, OUTPUT);
  pinMode(XSHUT2, OUTPUT);

  // Sensor 1 initialization
  digitalWrite(XSHUT1, LOW); // Turn off Sensor 1
  digitalWrite(XSHUT2, HIGH); // Ensure Sensor 2 is off
  delay(10);
  digitalWrite(XSHUT1, HIGH); // Turn on Sensor 1
  delay(10);
  if (!lox1.begin(0x30)) { // Assign a unique address (e.g., 0x30)
    Serial.println(F("Failed to boot VL53L0X #1"));
    while (1);
  }

  // Sensor 2 initialization
  digitalWrite(XSHUT2, LOW); // Turn off Sensor 2
  delay(10);
  digitalWrite(XSHUT2, HIGH); // Turn on Sensor 2
  delay(10);
  if (!lox2.begin(0x31)) { // Assign a unique address (e.g., 0x31)
    Serial.println(F("Failed to boot VL53L0X #2"));
    while (1);
  }

  Serial.println(F("VL53L0X Sensors Initialized"));
}

void loop() {
    VL53L0X_RangingMeasurementData_t measure1; // Separate instance for Sensor 1
    VL53L0X_RangingMeasurementData_t measure2; // Separate instance for Sensor 2

    // Sensor 1 Measurement
    lox1.rangingTest(&measure1, false);
    Serial.print("Sensor 1 Distance (mm): ");
    if (measure1.RangeStatus != 4) {
        Serial.println(measure1.RangeMilliMeter);
    } else {
        Serial.println(" out of range ");
    }

    // Sensor 2 Measurement
    lox2.rangingTest(&measure2, false);
    Serial.print("Sensor 2 Distance (mm): ");
    if (measure2.RangeStatus != 4) {
        Serial.println(measure2.RangeMilliMeter);
    } else {
        Serial.println(" out of range ");
    }

    delay(500); // Add delay to avoid rapid reads
}
