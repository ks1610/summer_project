#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// Define a simple stack
#define MAX_STACK_SIZE 10
char moveStack[MAX_STACK_SIZE];
int stackIndex = -1;

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

int motor1pin1 = 10;
int motor1pin2 = 11;
int motor2pin1 = 12;
int motor2pin2 = 13;

// Function to check if the stack is empty
bool isStackEmpty() {
  return stackIndex == -1;
}

// Function to push an element onto the stack
bool push(char move) {
  if (stackIndex < MAX_STACK_SIZE - 1) {
    moveStack[++stackIndex] = move;
    return true;
  }
  return false;
}

// Function to pop an element from the stack
char pop() {
  if (!isStackEmpty()) {
    return moveStack[stackIndex--];
  }
  return '\0'; // Return null character if stack is empty
}

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
  Serial.println("Moving forward...");
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
  delay(300);
}

void Move_backward() {
  Serial.println("Moving backward...");
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  delay(300);
}

void Move_left() {
  Serial.println("Turning left...");
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  delay(200);
}

void Move_right() {
  Serial.println("Turning right...");
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
  
  // (Optional)
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

int moveAttemptCount = 0;  // Counter for left/right turns and forward movements

int turnAttemptCount = 0;  // Counter for left/right turns

unsigned long frontWallStartTime = 0;  // Timer to track the wall detection duration
const unsigned long wallDetectDuration = 1000; // 1 seconds to detect wall (in milliseconds)

void loop() {
  // Set motor speeds
  analogWrite(A0, 255); // ENA pin
  analogWrite(A1, 130); // ENB pin

  // Read distances from sensors
  VL53L0X_RangingMeasurementData_t measureFront;
  lox[0].rangingTest(&measureFront, false); // Front sensor
  int frontDistance = (measureFront.RangeStatus != 4) ? measureFront.RangeMilliMeter : 0;

  int leftDistance = readUltrasonicDistance(0);  // Left ultrasonic sensor
  int rightDistance = readUltrasonicDistance(1); // Right ultrasonic sensor

  // Wall detection logic
  bool isFrontWall = (frontDistance < 48);  // Less than 48mm in front
  bool isLeftWall = (leftDistance <= 8 || leftDistance >= 1189);  // Left side wall
  bool isRightWall = (rightDistance <= 8 || rightDistance >= 1189); // Right side wall

  // Dead point detection using the front sensor
  if (isFrontWall) {
    if (frontWallStartTime == 0) {
      frontWallStartTime = millis();  // Start the timer when a wall is detected
    } else if (millis() - frontWallStartTime >= wallDetectDuration) {
      // If the front distance remains the same for 2 seconds, treat it as a wall
      if (frontDistance <= 15) {
        // Consider this a dead point, move backward
        Serial.println("Dead point detected, backtracking...");
        Move_backward();
        frontWallStartTime = 0;  // Reset the timer
        return; // Skip further checks and allow backtracking
      }
    }
  } else {
    frontWallStartTime = 0;  // Reset the timer if the wall is no longer detected
  }

  // Regular navigation logic
  if (isFrontWall && isLeftWall && isRightWall) {
    // Dead point detected, backtrack
    if (!isStackEmpty()) {
      char lastTurn = pop(); // Get the last recorded turn

      // Move backward
      Move_backward();

      // Reverse the last turn
      if (lastTurn == 'R') {
        Move_left();
      } else if (lastTurn == 'L') {
        Move_right();
      }
      
      // After backtracking, check available left and right
      leftDistance = readUltrasonicDistance(0);  // Left ultrasonic sensor
      rightDistance = readUltrasonicDistance(1); // Right ultrasonic sensor
      
      // Compare left and right paths
      if (leftDistance > rightDistance) {
        // If left is clearer, turn left
        Move_left();
        if (!push('L')) {
          // Stack overflow error handling
        }
      } else {
        // If right is clearer or same, turn right
        Move_right();
        if (!push('R')) {
          // Stack overflow error handling
        }
      }

      Move_forward(); // Continue moving forward after turning
    } else {
      // No moves left to backtrack
      analogWrite(A0, 0); // Stop motors
      analogWrite(A1, 0);
      while (1); // Halt the robot
    }
  } else {
    // Regular navigation logic
    if (!isFrontWall) {
      // Path is clear ahead, move forward
      Move_forward();
    } else {
      // Check the left side if the front is blocked
      if (!isLeftWall) {
        // Path is clear on the left, turn left
        Move_left();
        if (!push('L')) {
          // Stack overflow error handling
        }
        Move_forward(); // Continue moving forward after turning
      } else if (!isRightWall) {
        // Path is clear on the right, turn right
        Move_right();
        if (!push('R')) {
          // Stack overflow error handling
        }
        Move_forward(); // Continue moving forward after turning
      }
    }
  }

  delay(100); // Short delay for stability
}





