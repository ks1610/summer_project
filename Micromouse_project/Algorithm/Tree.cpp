#include <Wire.h>
#include <VL53L0X.h>

// Sensor objects
VL53L0X sensorFront;
VL53L0X sensorBack;

// Pin definitions
#define XSHUT1 8
#define XSHUT2 9
const uint8_t XSHUT_PINS[2] = {XSHUT1, XSHUT2};

// Ultrasonic sensor pin definitions
unsigned int TRIG_PIN[2] = {2, 4};
unsigned int ECHO_PIN[2] = {3, 5};

// Motor pin definitions
int motor1pin1 = 10;
int motor1pin2 = 11;
int motor2pin1 = 12;
int motor2pin2 = 13;

// Constants
const unsigned int BAUD_RATE = 9600;
const int FRONT_WALL_THRESHOLD = 48; // mm
const int SIDE_WALL_MIN_THRESHOLD = 8;  // cm
const int SIDE_WALL_MAX_THRESHOLD = 1189; // cm
const int MOTOR_DELAY = 1000; // Adjusted delay for motor actions

// Struct for binary tree nodes
struct TreeNode {
    char move;          // 'F', 'L', 'R'
    TreeNode *left;     // Left branch
    TreeNode *right;    // Right branch
    TreeNode(char m) : move(m), left(nullptr), right(nullptr) {}
};

// Initialize sensors
void setupSensors() {
    pinMode(XSHUT1, OUTPUT);
    pinMode(XSHUT2, OUTPUT);

    // Reset sensors
    digitalWrite(XSHUT1, LOW);
    digitalWrite(XSHUT2, LOW);
    delay(10);

    // Initialize front sensor
    digitalWrite(XSHUT1, HIGH);
    delay(10);
    sensorFront.setTimeout(500);
    sensorFront.init();
    sensorFront.startContinuous();

    // Initialize back sensor
    digitalWrite(XSHUT2, HIGH);
    delay(10);
    sensorBack.setTimeout(500);
    sensorBack.init();
    sensorBack.startContinuous();

    // Ultrasonic pins
    for (int i = 0; i < 2; i++) {
        pinMode(TRIG_PIN[i], OUTPUT);
        pinMode(ECHO_PIN[i], INPUT);
    }
}

// Motor setup
void setupMotors() {
    pinMode(motor1pin1, OUTPUT);
    pinMode(motor1pin2, OUTPUT);
    pinMode(motor2pin1, OUTPUT);
    pinMode(motor2pin2, OUTPUT);

    // (Optional)
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);
}

// Utility functions to read sensors
int readDistanceFront() {
    int distance = sensorFront.readRangeContinuousMillimeters();
    Serial.print("Front Distance: ");
    Serial.println(distance);
    return distance;
}

int readDistanceBack() {
    int distance = sensorBack.readRangeContinuousMillimeters();
    Serial.print("Back Distance: ");
    Serial.println(distance);
    return distance;
}

int readDistanceUltrasonic(int sensorIndex) {
    digitalWrite(TRIG_PIN[sensorIndex], LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN[sensorIndex], HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN[sensorIndex], LOW);
    int distance = pulseIn(ECHO_PIN[sensorIndex], HIGH) / 58; // Convert to cm
    Serial.print(sensorIndex == 0 ? "Left Distance: " : "Right Distance: ");
    Serial.println(distance);
    return distance;
}

bool detectWallFront() {
    int distance = readDistanceFront();
    return (distance > 0 && distance < FRONT_WALL_THRESHOLD);
}

bool detectWallLeft() {
    int distance = readDistanceUltrasonic(0);
    return distance >= SIDE_WALL_MIN_THRESHOLD && distance <= SIDE_WALL_MAX_THRESHOLD;
}

bool detectWallRight() {
    int distance = readDistanceUltrasonic(1);
    return distance >= SIDE_WALL_MIN_THRESHOLD && distance <= SIDE_WALL_MAX_THRESHOLD;
}

// Movement control functions
void moveForward() {
    Serial.println("Moving Forward");
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
    delay(MOTOR_DELAY); // Adjusted delay for smoother movement
}

void turnLeft() {
    Serial.println("Turning Left");
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH);
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
    delay(MOTOR_DELAY); // Adjusted for 90-degree turn
}

void turnRight() {
    Serial.println("Turning Right");
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);
    delay(MOTOR_DELAY); // Adjusted for 90-degree turn
}

void moveBackward() {
    Serial.println("Moving Backward");
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);
    delay(MOTOR_DELAY); // Adjusted for appropriate distance
}

bool isDestination(TreeNode* current) {
    return false; // Remove destination logic for simplified movement
}

TreeNode* explore(TreeNode* current) {
    if (!detectWallFront()) {
        Serial.println("Path forward detected");
        current->right = new TreeNode('F');
        moveForward();
        return current->right;
    } else if (!detectWallLeft()) {
        Serial.println("Path left detected");
        current->left = new TreeNode('L');
        turnLeft();
        return current->left;
    } else if (!detectWallRight()) {
        Serial.println("Path right detected");
        current->left = new TreeNode('R');
        turnRight();
        return current->left;
    }
    Serial.println("Dead end, moving backward");
    moveBackward();
    return nullptr;
}

TreeNode* backtrack(TreeNode* current, TreeNode* parent) {
    Serial.println("Backtracking");
    moveBackward();
    return parent;
}

void solveMaze() {
    Serial.println("Starting maze solving");
    TreeNode* root = new TreeNode('S');
    TreeNode* current = root;
    TreeNode* parent = nullptr;

    while (true) { // Infinite loop to keep solving
        TreeNode* next = explore(current);
        if (next) {
            parent = current;
            current = next;
        } else {
            current = backtrack(current, parent);
        }
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    setupSensors();
    setupMotors();
    Serial.println("Setup complete");
}

void loop() {
    // Set motor speeds
    analogWrite(A0, 200); // ENA pin
    analogWrite(A1, 150); // ENB pin

    solveMaze();
}
