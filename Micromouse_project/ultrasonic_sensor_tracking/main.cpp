
unsigned int TRIG_PIN[4] = {8, 2, 6, 4};
unsigned int ECHO_PIN[4] = {9, 3, 7, 5};

const unsigned int BAUD_RATE = 9600;

void setup() {
  Serial.begin(BAUD_RATE);
  for(int i = 0; i < 4; i++){
    pinMode(TRIG_PIN[i], OUTPUT);
    pinMode(ECHO_PIN[i], INPUT);
  }
}

void loop()  {
  unsigned long duration[4];
  int distance[4];

  for(int i = 0; i < 4; i++){
    digitalWrite(TRIG_PIN[i], LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN[i], LOW);

    // Measure the pulse duration
    duration[i] = pulseIn(ECHO_PIN[i], HIGH);
    distance[i] = duration[i] / 29 / 2; // Convert to distance in cm
  }

  // Print distances for each sensor
  for(int i = 0; i < 4; i++) {
    if (duration[i] == 0) {
      Serial.print("Warning: no pulse from sensor ");
      Serial.print(i);
      Serial.println();
    } else {
      Serial.print("Distance from sensor ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(distance[i]);
      Serial.println(" cm");
    }
  }

  delay(1000); // Delay between readings
}
