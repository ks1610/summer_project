// Pin definitions for ultrasonic sensors
const int trigPins[4] = {2, 3, 4, 5};
const int echoPins[4] = {6, 7, 8, 9};

// Motor control pins
const int leftMotorForward = 10;
const int leftMotorBackward = 11;
const int rightMotorForward = 12;
const int rightMotorBackward = 13;

// Robot state
int robotX = 0, robotY = 0; // Starting position
int robotDir = 0;           // 0 = North, 1 = East, 2 = South, 3 = West
bool visited[10][10];       // 10x10 grid of visited cells
int backtrackStack[100][2]; // Stack for backtracking
int stackSize = 0;          // Current stack size

// Function to initialize pins
void setup() {
  Serial.begin(9600);

  // Initialize ultrasonic sensors
  for (int i = 0; i < 4; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  // Initialize motor pins
  pinMode(leftMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);

  // Initialize visited grid
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      visited[i][j] = false;
    }
  }
  visited[robotX][robotY] = true;

  // Push starting position to the backtrack stack
  backtrackStack[stackSize][0] = robotX;
  backtrackStack[stackSize][1] = robotY;
  stackSize++;
}

// Function to get distance from an ultrasonic sensor
long getDistance(int sensorIndex) {
  digitalWrite(trigPins[sensorIndex], LOW);
  delayMicroseconds(2);
  digitalWrite(trigPins[sensorIndex], HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPins[sensorIndex], LOW);

  long duration = pulseIn(echoPins[sensorIndex], HIGH);
  return duration * 0.034 / 2; // Convert to cm
}

// Function to move the robot
void moveForward() {
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(rightMotorForward, HIGH);
  delay(500);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, LOW);
}

// Function to turn the robot
void turnRight() {
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(rightMotorBackward, HIGH);
  delay(500);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorBackward, LOW);
}

void turnLeft() {
  digitalWrite(leftMotorBackward, HIGH);
  digitalWrite(rightMotorForward, HIGH);
  delay(500);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorForward, LOW);
}

void moveBackward() {
  digitalWrite(leftMotorBackward, HIGH);
  digitalWrite(rightMotorBackward, HIGH);
  delay(500);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorBackward, LOW);
}

// Main loop
void loop() {
  long distances[4];
  for (int i = 0; i < 4; i++) {
    distances[i] = getDistance(i);
  }

  Serial.print("Distances: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(distances[i]);
    Serial.print(" ");
  }
  Serial.println();

  // Decision-making
  bool moved = false;
  for (int i = 0; i < 4; i++) {
    int direction = (robotDir + i) % 4;
    if (distances[direction] > 15) { // Check if the path is clear
      if (i == 0) {
        moveForward();
      } else if (i == 1) {
        turnRight();
        moveForward();
      } else if (i == 3) {
        turnLeft();
        moveForward();
      }
      moved = true;
      break;
    }
  }

  // Backtrack if no move possible
  if (!moved) {
    Serial.println("Dead end! Backtracking...");
    stackSize--;
    if (stackSize > 0) {
      robotX = backtrackStack[stackSize - 1][0];
      robotY = backtrackStack[stackSize - 1][1];
      moveBackward();
    } else {
      Serial.println("Maze solved!");
      while (1); // Stop
    }
  }

  delay(500);
}
