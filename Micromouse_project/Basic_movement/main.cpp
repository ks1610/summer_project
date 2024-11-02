int motor1pin1 = 10;
int motor1pin2 = 11;
int motor2pin1 = 12;
int motor2pin2 = 13;

unsigned int TRIG_PIN[4] = {8, 2, 6, 4};
unsigned int ECHO_PIN[4] = {9, 3, 7, 5};

const unsigned int BAUD_RATE = 9600;

void setup() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  //(Optional)
  pinMode(A1, OUTPUT); 
  pinMode(A2, OUTPUT);

  for(int i = 0; i < 4; i++){
    pinMode(TRIG_PIN[i], OUTPUT);
    pinMode(ECHO_PIN[i], INPUT);
  }

  Serial.begin(BAUD_RATE);
}

void Move_forward() {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  delay(300);
}

void Move_backward() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
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

int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH);
  return duration / 29 / 2; // Convert to distance in cm
}

void loop() {
  // Get distances from each sensor
  int distance_front = getDistance(TRIG_PIN[0], ECHO_PIN[0]);
  int distance_right = getDistance(TRIG_PIN[1], ECHO_PIN[1]);
  int distance_rear = getDistance(TRIG_PIN[2], ECHO_PIN[2]);
  int distance_left = getDistance(TRIG_PIN[3], ECHO_PIN[3]);

  // Set motor speeds (0 = off, 255 = max speed)
  analogWrite(A1, 255); // ENA pin
  analogWrite(A2, 255); // ENB pin

  if (distance_front < 10) {
    // If both sides are blocked, move backward
    if (distance_left < 10 || distance_left > 1190 && distance_right < 10 || distance_right > 1190) {
      Move_backward();
    }
    // If only the left side is clearer, turn left
    else if (distance_left > distance_right) {
      Move_left();
    }
    // If only the right side is clearer, turn right
    else {
      Move_right();
    }
  } 
  else if (distance_left < 10 || distance_left > 1190) {
    // If there is an obstacle on the left, turn slightly right
    Move_right();
  }
  else if (distance_right < 10 || distance_right > 1190) {
    // If there is an obstacle on the right, turn slightly left
    Move_left();
  }
  else {
    // Move forward if no obstacles are detected
    Move_forward();
  }
}
