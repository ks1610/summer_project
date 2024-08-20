#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

//ultra sonic sensor 
#define echoPin 6

#define trigPin 7

/*long duration;

int distance;
*/
/*
//joystick control
#define joyX A0

#define joyY A1


int button=2;

int buttonState = 0;

int buttonState1 = 0;
*/
void setup() {
  // attaches the servo on pin 9 to the servo object
  myservo.attach(9);  
  
  // attaches  ultrasonic sensor
  pinMode(trigPin, OUTPUT);

  pinMode(echoPin, INPUT);

  // joystick
  /*pinMode(7,OUTPUT);

  pinMode(button,INPUT);

  digitalWrite(button, HIGH);

  Serial.begin(9600);    */
}

void loop() {
  /*for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }*/
/*
  digitalWrite(trigPin, LOW);

  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);

  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.034 / 2;

*/
long duration, distance;

digitalWrite(trigPin, LOW);

delayMicroseconds(2);

digitalWrite(trigPin, HIGH);

delayMicroseconds(10);

digitalWrite(trigPin, LOW);

duration = pulseIn(echoPin, HIGH);

distance = (duration/2) / 29.1;

  if(distance <= 10){
    myservo.write(50);

    Serial.print("Distance: ");

    Serial.print(distance);

    Serial.println(" cm");
    delay(5000);
  }
  else {

    myservo.write(140);

    Serial.print("Distance: ");

    Serial.print(distance);

    Serial.println(" cm");

  }

delay(50);

/*
 int xValue = analogRead(joyX);

 int yValue = analogRead(joyY);

 

  Serial.print(xValue);

  Serial.print("\t");

  Serial.println(yValue);

  

  buttonState = digitalRead(button);

  Serial.println(buttonState);

  if (xValue>=0 && yValue<=10)

  {

    digitalWrite(10, HIGH);

  }

  else{digitalWrite(10, LOW);}


  if (xValue<=10 && yValue>=500)

  {

    digitalWrite(11, HIGH);

  }

  else{digitalWrite(11, LOW);}


  if (xValue>=1020 && yValue>=500)

  {

    digitalWrite(9, HIGH);

  }

  else{digitalWrite(9, LOW);}


  if (xValue>=500 && yValue>=1020)

  {

    digitalWrite(8, HIGH);

  }

  else{digitalWrite(8, LOW);}


  if (xValue>=1020 && yValue>=1020)

  {

    digitalWrite(9, LOW);

    digitalWrite(8, LOW);

  }


  if (buttonState == LOW)

  {

    Serial.println("Switch = High");

    digitalWrite(7, HIGH);

  }

  else{digitalWrite(7, LOW);}

  buttonState1 = digitalRead(7);

  Serial.println(buttonState1);

  delay(50);
*/
}
