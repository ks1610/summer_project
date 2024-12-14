// Motor pins
int motor1pin1 = 10; // Motor 1 forward
int motor1pin2 = 11; // Motor 1 backward
int motor2pin1 = 12; // Motor 2 forward
int motor2pin2 = 13; // Motor 2 backward

// Sensor pins
unsigned int TRIG_PIN[4] = {8, 2, 6, 4};
unsigned int ECHO_PIN[4] = {9, 3, 7, 5};

// Maze and robot state
const unsigned int MAZE_SIZE = 10; // Example maze size
int maze[MAZE_SIZE][MAZE_SIZE][3]; // 3 values for [left, front, right]
int robot_x = 0, robot_y = 0;      // Current position in the maze
int robot_dir = 0;                 // 0 = North, 1 = East, 2 = South, 3 = West

// Custom queue for unexplored paths
const unsigned int QUEUE_SIZE = 50; // Max number of unexplored paths
int queueX[QUEUE_SIZE], queueY[QUEUE_SIZE]; // Arrays for x and y coordinates
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

  // Treat distances larger than 1190 cm as walls
  maze[robot_x][robot_y][0] = (left < 5 || left > 1190) ? 1 : 0;
  maze[robot_x][robot_y][1] = (front < 5 || front > 1190) ? 1 : 0;
  maze[robot_x][robot_y][2] = (right < 5 || right > 1190) ? 1 : 0;

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

  // If front is open, move forward
  if (maze[robot_x][robot_y][1] == 0) {
    moveRobot(robot_dir);
  } else if (maze[robot_x][robot_y][0] == 0) {
    // If left is open, turn left and move
    robot_dir = (robot_dir + 3) % 4; // Turn left
    Move_left();
    moveRobot(robot_dir);
  } else if (maze[robot_x][robot_y][2] == 0) {
    // If right is open, turn right and move
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
    while (true); // Stop the program
  }
}


// Backtrack to a specific cell
void backtrackTo(int target_x, int target_y) {
  while (robot_x != target_x || robot_y != target_y) {
    Serial.println("Backtracking...");
    Move_backward();
    if (robot_x > target_x) robot_x--;
    else if (robot_x < target_x) robot_x++;
    if (robot_y > target_y) robot_y--;
    else if (robot_y < target_y) robot_y++;
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

  // Treat distances greater than 1190 cm as 0 cm
  if (distance > 1189) {
    distance = 0;
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
  int distance_front = getDistance(TRIG_PIN[0], ECHO_PIN[0]);
  int distance_right = getDistance(TRIG_PIN[1], ECHO_PIN[1]);
  int distance_rear = getDistance(TRIG_PIN[2], ECHO_PIN[2]);
  int distance_left = getDistance(TRIG_PIN[3], ECHO_PIN[3]);

  Serial.println("Moving forward...");
  Serial.print("Front: ");
  Serial.print(distance_front);
  Serial.print(" cm, Right: ");
  Serial.print(distance_right);
  Serial.print(" cm, Rear: ");
  Serial.print(distance_rear);
  Serial.print(" cm, Left: ");
  Serial.print(distance_left);
  Serial.println(" cm");

  delay(300);
}

void Move_backward() {
  int distance_front = getDistance(TRIG_PIN[0], ECHO_PIN[0]);
  int distance_right = getDistance(TRIG_PIN[1], ECHO_PIN[1]);
  int distance_rear = getDistance(TRIG_PIN[2], ECHO_PIN[2]);
  int distance_left = getDistance(TRIG_PIN[3], ECHO_PIN[3]);

  Serial.println("Moving backward...");
  Serial.print("Front: ");
  Serial.print(distance_front);
  Serial.print(" cm, Right: ");
  Serial.print(distance_right);
  Serial.print(" cm, Rear: ");
  Serial.print(distance_rear);
  Serial.print(" cm, Left: ");
  Serial.print(distance_left);
  Serial.println(" cm");

  delay(300);
}

void Move_left() {
  int distance_front = getDistance(TRIG_PIN[0], ECHO_PIN[0]);
  int distance_right = getDistance(TRIG_PIN[1], ECHO_PIN[1]);
  int distance_rear = getDistance(TRIG_PIN[2], ECHO_PIN[2]);
  int distance_left = getDistance(TRIG_PIN[3], ECHO_PIN[3]);

  Serial.println("Turning left...");
  Serial.print("Front: ");
  Serial.print(distance_front);
  Serial.print(" cm, Right: ");
  Serial.print(distance_right);
  Serial.print(" cm, Rear: ");
  Serial.print(distance_rear);
  Serial.print(" cm, Left: ");
  Serial.print(distance_left);
  Serial.println(" cm");

  delay(200);
}

void Move_right() {
  int distance_front = getDistance(TRIG_PIN[0], ECHO_PIN[0]);
  int distance_right = getDistance(TRIG_PIN[1], ECHO_PIN[1]);
  int distance_rear = getDistance(TRIG_PIN[2], ECHO_PIN[2]);
  int distance_left = getDistance(TRIG_PIN[3], ECHO_PIN[3]);

  Serial.println("Turning right...");
  Serial.print("Front: ");
  Serial.print(distance_front);
  Serial.print(" cm, Right: ");
  Serial.print(distance_right);
  Serial.print(" cm, Rear: ");
  Serial.print(distance_rear);
  Serial.print(" cm, Left: ");
  Serial.print(distance_left);
  Serial.println(" cm");

  delay(200);
}
*/

void Move_forward() {
  digitalWrite(motor1pin1, HIGH); // Motor 1 forward
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH); // Motor 2 forward
  digitalWrite(motor2pin2, LOW);

  Serial.println("Moving forward...");
  delay(500); // Adjust duration as needed
  distance_tracking();
  // Stop the motors after the movement
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor2pin1, LOW);
}

void Move_backward() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH); // Motor 1 backward
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH); // Motor 2 backward

  Serial.println("Moving backward...");
  delay(500); // Adjust duration as needed
  distance_tracking();

  // Stop the motors after the movement
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin2, LOW);
}

void Move_left() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH); // Motor 1 backward
  digitalWrite(motor2pin1, HIGH); // Motor 2 forward
  digitalWrite(motor2pin2, LOW);

  Serial.println("Turning left...");
  delay(300); // Adjust duration for a turn
  distance_tracking();

  // Stop the motors after the turn
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
}

void Move_right() {
  digitalWrite(motor1pin1, HIGH); // Motor 1 forward
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH); // Motor 2 backward

  Serial.println("Turning right...");
  delay(300); // Adjust duration for a turn
  distance_tracking();

  // Stop the motors after the turn
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}
