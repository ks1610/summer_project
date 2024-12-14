// Motor pins
int motor1pin1 = 10; // Motor 1 forward
int motor1pin2 = 11; // Motor 1 backward
int motor2pin1 = 12; // Motor 2 forward
int motor2pin2 = 13; // Motor 2 backward

// Sensor pins
unsigned int TRIG_PIN[4] = {8, 2, 6, 4};
unsigned int ECHO_PIN[4] = {9, 3, 7, 5};

// Maze and robot state
const unsigned int MAZE_SIZE = 8;    // Reduce maze size
uint8_t maze[MAZE_SIZE][MAZE_SIZE][3]; // Use 1 byte per element

int robot_x = 0, robot_y = 0;      // Current position in the maze
int robot_dir = 0;                 // 0 = North, 1 = East, 2 = South, 3 = West

// Custom queue for unexplored paths
const unsigned int QUEUE_SIZE = 200; // Reduce queue size
uint8_t queueX[QUEUE_SIZE], queueY[QUEUE_SIZE]; // Use 1 byte per coordinate

int front = -1, rear = -1;

// Function prototypes
void enqueue(int x, int y);
void dequeue(int &x, int &y);
bool isQueueEmpty();
void updateMaze();
void moveRobot(int direction);
void exploreMaze();
void backtrackTo(int target_x, int target_y);
int getDistance(int trigPin, int echoPin);
void Move_forward();
void Move_backward();
void Move_left();
void Move_right();

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(TRIG_PIN[i], OUTPUT);
    pinMode(ECHO_PIN[i], INPUT);
  }

  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  //(Optional)
  pinMode(A1, OUTPUT); 
  pinMode(A2, OUTPUT);

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
  int left = getDistance(TRIG_PIN[3], ECHO_PIN[3]);
  int front = getDistance(TRIG_PIN[0], ECHO_PIN[0]);
  int right = getDistance(TRIG_PIN[1], ECHO_PIN[1]);

  // Treat 0 cm as a blocked path only if it persists
  maze[robot_x][robot_y][0] = (left <= 8 && left > 0) ? 1 : 0;  // Left
  maze[robot_x][robot_y][1] = (front <= 8 && front > 0) ? 1 : 0; // Front
  maze[robot_x][robot_y][2] = (right <= 8 && right > 0) ? 1 : 0; // Right

  // Enqueue unexplored paths if the corresponding direction is open
  if (maze[robot_x][robot_y][0] == 0 && robot_x - 1 >= 0) enqueue(robot_x - 1, robot_y); // Left
  if (maze[robot_x][robot_y][1] == 0 && robot_y + 1 < MAZE_SIZE) enqueue(robot_x, robot_y + 1); // Front
  if (maze[robot_x][robot_y][2] == 0 && robot_x + 1 < MAZE_SIZE) enqueue(robot_x + 1, robot_y); // Right
}

// Move the robot in a specific direction
void moveRobot(int direction) {
  switch (direction) {
    case 0: Move_forward(); robot_y++; break;  // North
    case 1: Move_right(); robot_x++; break;   // East
    case 2: Move_backward(); robot_y--; break; // South
    case 3: Move_left(); robot_x--; break;    // West
  }
}

// Explore the maze
void exploreMaze() {
  updateMaze();

  // Check distances for each direction and move accordingly
  int frontDist = getDistance(TRIG_PIN[0], ECHO_PIN[0]);
  int leftDist = getDistance(TRIG_PIN[3], ECHO_PIN[3]);
  int rightDist = getDistance(TRIG_PIN[1], ECHO_PIN[1]);

  if (maze[robot_x][robot_y][1] == 0 && frontDist > 0) {
    // If front is open and not blocked (distance > 0), move forward
    moveRobot(robot_dir);
  } else if (maze[robot_x][robot_y][0] == 0 && leftDist > 0) {
    // If left is open and not blocked, turn left and move
    robot_dir = (robot_dir + 3) % 4; // Turn left
    Move_left();
    moveRobot(robot_dir);
  } else if (maze[robot_x][robot_y][2] == 0 && rightDist > 0) {
    // If right is open and not blocked, turn right and move
    robot_dir = (robot_dir + 1) % 4; // Turn right
    Move_right();
    moveRobot(robot_dir);
  } else if (!isQueueEmpty()) {
    // If all directions are blocked, backtrack to last unexplored path
    int next_x, next_y;
    dequeue(next_x, next_y);
    backtrackTo(next_x, next_y);
  } else {
    // No more paths to explore
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
      maze[i][j][2] = 0;
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
    int rearDist = getDistance(TRIG_PIN[2], ECHO_PIN[2]); // Rear distance check
    if (rearDist > 0) {
      Serial.println("Backtracking...");
      Move_backward();
      if (robot_x > target_x) robot_x--;
      else if (robot_x < target_x) robot_x++;
      if (robot_y > target_y) robot_y--;
      else if (robot_y < target_y) robot_y++;
    } else {
      Serial.println("Blocked behind! Cannot backtrack.");
      break; // Stop backtracking if rear is blocked
    }
  }
}

// Main loop
void loop() {  
  
  // Set motor speeds (0 = off, 255 = max speed)
  analogWrite(A1, 255); // ENA pin
  analogWrite(A2, 255); // ENB pin
  
  exploreMaze();
}

// Ultrasonic sensor distance
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH);
  int distance = duration / 29 / 2; // Convert to distance in cm

  // Retry logic if distance is 0 or out of range
  for (int i = 0; i < 3; i++) {
    if (distance > 0 && distance < 1180) break; // Valid range: 1 cm to 1199 cm

    // Retry if invalid
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration / 29 / 2;
  }

  // Treat persistent 0 cm or out-of-range values as blocked
  if (distance <= 0 || distance >= 1180) {
    distance = 0; // Blocked
  }

  return distance;
}


void distance_tracking(){
  int distance_front = getDistance(TRIG_PIN[0], ECHO_PIN[0]);
  int distance_right = getDistance(TRIG_PIN[1], ECHO_PIN[1]);
  int distance_rear = getDistance(TRIG_PIN[2], ECHO_PIN[2]);
  int distance_left = getDistance(TRIG_PIN[3], ECHO_PIN[3]);

  Serial.print("Front: ");
  Serial.print(distance_front);
  Serial.print(" cm, Right: ");
  Serial.print(distance_right);
  Serial.print(" cm, Rear: ");
  Serial.print(distance_rear);
  Serial.print(" cm, Left: ");
  Serial.print(distance_left);
  Serial.println(" cm");
}

/*
void Move_forward() {
  int frontDist = getDistance(TRIG_PIN[0], ECHO_PIN[0]);
  if (frontDist > 7) {
    Serial.println("Moving forward...");
    distance_tracking();
    delay(300);
  } else {
    Serial.println("Path blocked! Stopping...");
  }
}

void Move_backward() {
  int rearDist = getDistance(TRIG_PIN[2], ECHO_PIN[2]); // Check rear distance
  if (rearDist > 7) {
    Serial.println("Moving backward...");
    distance_tracking();
    delay(300);
  } else {
    Serial.println("Path blocked behind! Stopping...");
    // Optionally, enqueue another exploration path or stop completely
  }
}

void Move_left() {
  Serial.println("Turning left...");
  distance_tracking();
  delay(200);
}

void Move_right() {
  Serial.println("Turning right...");
  distance_tracking();
  delay(200);
}
*/


void Move_forward() {
  int frontDist = getDistance(TRIG_PIN[0], ECHO_PIN[0]);
  if (frontDist > 7) {
    Serial.println("Moving forward...");
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
    delay(500); // Adjust duration
  } else {
    Serial.println("Path blocked! Stopping...");
  }
}

void Move_backward() {
  int rearDist = getDistance(TRIG_PIN[2], ECHO_PIN[2]); // Check rear distance
  if (rearDist > 7) {
    Serial.println("Moving backward...");
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH); // Motor 1 backward
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH); // Motor 2 backward
    delay(500); // Adjust duration as needed
  } else {
    Serial.println("Path blocked behind! Stopping...");
    // Optionally, enqueue another exploration path or stop completely
  }
}


void Move_left() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH); // Motor 1 backward
  
  digitalWrite(motor2pin1, HIGH); // Motor 2 forward
  digitalWrite(motor2pin2, LOW);

  delay(300); // Adjust duration for a turn
}

void Move_right() {
  digitalWrite(motor1pin1, HIGH); // Motor 1 forward
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH); // Motor 2 backward

  delay(300); // Adjust duration for a turn

}
