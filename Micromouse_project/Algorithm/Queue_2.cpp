#include <Wire.h>
#include <Adafruit_VL53L0X.h>

// Motor pins
int motor1pin1 = 10; // Motor 1 forward
int motor1pin2 = 11; // Motor 1 backward
int motor2pin1 = 12; // Motor 2 forward
int motor2pin2 = 13; // Motor 2 backward

// Motor enable pins
#define ENA_PIN A0
#define ENB_PIN A1

// VL53L0X sensors for front and back
Adafruit_VL53L0X lox[2] = {Adafruit_VL53L0X(), Adafruit_VL53L0X()};

// XSHUT pin definitions
#define XSHUT1 8
#define XSHUT2 9
const uint8_t XSHUT_PINS[2] = {XSHUT1, XSHUT2};

// Maze and robot state
const unsigned int MAZE_SIZE = 4;    // Reduce maze size
const unsigned int QUEUE_SIZE = 100; // Reduce queue size
// Removed PROGMEM, now it is a normal array in SRAM
uint8_t maze[MAZE_SIZE][MAZE_SIZE][3]; // Store maze data in SRAM

int robot_x = 0, robot_y = 0;      // Current position in the maze
int robot_dir = 0;                 // 0 = North, 1 = East, 2 = South, 3 = West

// Custom queue for unexplored paths
uint8_t queueX[QUEUE_SIZE], queueY[QUEUE_SIZE];
int front = -1, rear = -1;

// Motor speed (0 = off, 255 = max speed)
#define MOTOR_SPEED 255

// Function prototypes
void enqueue(int x, int y);
void dequeue(int &x, int &y);
bool isQueueEmpty();
void updateMaze();
void moveRobot(int direction);
void exploreMaze();
void backtrackTo(int target_x, int target_y);
int getDistance(int sensorIndex);
void moveForward();
void moveBackward();
void moveLeft();
void moveRight();
void resetMaze();

void setup() {
  // Initialize motor enable pins
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);

  // Set motor speeds (0 = off, 255 = max speed)
  analogWrite(ENA_PIN, MOTOR_SPEED);
  analogWrite(ENB_PIN, MOTOR_SPEED);

  // Set up the VL53L0X sensors
  for (int i = 0; i < 2; i++) {
    pinMode(XSHUT_PINS[i], OUTPUT);
    digitalWrite(XSHUT_PINS[i], LOW);
  }
  delay(100);
  digitalWrite(XSHUT1, HIGH);
  delay(100);
  if (!lox[0].begin()) {
    Serial.println("Failed to initialize front sensor!");
    while (1);
  }
  digitalWrite(XSHUT2, HIGH);
  delay(100);
  if (!lox[1].begin()) {
    Serial.println("Failed to initialize back sensor!");
    while (1);
  }

  // Motor pins
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  Serial.begin(9600);
}

// Custom queue operations
void enqueue(int x, int y) {
  if (rear == QUEUE_SIZE - 1) {
    Serial.println("Queue overflow!");
    return;
  }
  if (front == -1) front = 0; // Initialize front
  rear++;
  queueX[rear] = x;
  queueY[rear] = y;
}

void dequeue(int &x, int &y) {
  if (isQueueEmpty()) {
    Serial.println("Queue underflow!");
    return;
  }
  x = queueX[front];
  y = queueY[front];
  front++;
  if (front > rear) front = rear = -1; // Reset queue
}

bool isQueueEmpty() {
  return (front == -1);
}

// Update maze with distances from sensors
void updateMaze() {
  int front = getDistance(0);
  int back = getDistance(1);

  // Treat 0 cm as a blocked path only if it persists
  maze[robot_x][robot_y][0] = (front <= 8 && front > 0) ? 1 : 0;  // Front
  maze[robot_x][robot_y][1] = (back <= 8 && back > 0) ? 1 : 0; // Back

  // Enqueue unexplored paths if the corresponding direction is open
  if (maze[robot_x][robot_y][0] == 0 && robot_x - 1 >= 0) enqueue(robot_x - 1, robot_y); // Left
  if (maze[robot_x][robot_y][1] == 0 && robot_y + 1 < MAZE_SIZE) enqueue(robot_x, robot_y + 1); // Front
}

// Move the robot in a specific direction
void moveRobot(int direction) {
  switch (direction) {
    case 0: moveForward(); robot_y++; break;  // North
    case 1: moveRight(); robot_x++; break;   // East
    case 2: moveBackward(); robot_y--; break; // South
    case 3: moveLeft(); robot_x--; break;    // West
  }
}

// Explore the maze
void exploreMaze() {
  updateMaze();

  if (maze[robot_x][robot_y][1] == 0) {
    moveRobot(robot_dir); // Move forward
  } else if (maze[robot_x][robot_y][0] == 0) {
    moveRobot((robot_dir + 3) % 4); // Turn left
  } else if (!isQueueEmpty()) {
    int next_x, next_y;
    dequeue(next_x, next_y);
    backtrackTo(next_x, next_y);
  } else {
    Serial.println("Maze fully explored!");
    resetMaze(); // Clear maze data and start again
  }
}

// Function to reset the maze and robot state
void resetMaze() {
  // Clear the maze array
  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      maze[i][j][0] = 0;
      maze[i][j][1] = 0;
    }
  }

  // Reset the queue
  front = -1;
  rear = -1;

  // Reset robot position and direction
  robot_x = 0;
  robot_y = 0;
  robot_dir = 0;

  Serial.println("Maze reset. Starting exploration again...");
}

void backtrackTo(int target_x, int target_y) {
  while (robot_x != target_x || robot_y != target_y) {
    moveBackward();
  }
}

// Main loop
void loop() {
  exploreMaze();
}

// Function to get distance from VL53L0X sensors
int getDistance(int sensorIndex) {
  int distance = 0;
  lox[sensorIndex].startRangeContinuous(); // Start the continuous range measurement
  delay(50); // Give it time to get the measurement

  distance = lox[sensorIndex].readRange(); // Read the range result

  // Retry logic if distance is 0 or out of range
  for (int i = 0; i < 3; i++) {
    if (distance > 0 && distance < 1180) break;

    lox[sensorIndex].startRangeContinuous(); // Restart the range measurement
    delay(50);
    distance = lox[sensorIndex].readRange(); // Read the range result
  }

  return (distance <= 0 || distance >= 1180) ? 0 : distance; // Return 0 if out of range
}

// Motor control functions

// Move forward
void moveForward() {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

// Move backward
void moveBackward() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

// Turn left
void moveLeft() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

// Turn right
void moveRight() {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}
