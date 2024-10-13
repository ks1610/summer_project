#include <Servo.h>

const int buttonPin = 2;  // the number of the pushbutton pin
// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

// create 6 servo objects to control servos
Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;
Servo myservo5;
Servo myservo6; 

int joystick_x = A0;              // joystick x direction pin                                          
int joystick_y = A1;              // joystick y direction pin                                         
int pos_x;                   // declaring variable to stroe horizontal value
int pos_y;                   // declaring variable to stroe vertical value
int servo1_pos=90;          // storing servo position
int servo2_pos=90;


byte switchPin = 1;                    // switch is connected to pin 2
byte buttonPresses = 0;                // how many times the button has been pressed 

void setup() {

  Serial.begin (9600) ;

  // attach servos to their respective pins
  myservo1.attach(3);
  myservo2.attach(4);
  myservo3.attach(5);
  myservo4.attach(6);
  myservo5.attach(7);
  myservo6.attach(8);

  myservo1.write (servo1_pos);           
  myservo2.write (servo2_pos);
  pinMode (joystick_x, INPUT) ;                     
  pinMode (joystick_y, INPUT) ;     

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

void loop() {
  pos_x = analogRead (joystick_x) ;  
  pos_y = analogRead (joystick_y) ;                      

  if (pos_x < 300)            //if the horizontal value from joystick is less than 300
  {
    if (servo1_pos < 10)      //first servo moves right
    { 
    } 
    else
    { 
      servo1_pos = servo1_pos - 20; 
      myservo1.write ( servo1_pos ) ; 
      delay (100); 
    } 
  } 
  if (pos_x > 700)
  {
    if (servo1_pos > 180)
    {  
    }  
    else
    {
    servo1_pos = servo1_pos + 20;
    myservo1.write ( servo1_pos ) ;
    delay (100) ;
    }
  }

  if (pos_y < 300)      //if the vertical value from joystick is less than 300
  {
    if (servo2_pos < 10)  //second servo moves right
    { 
    } 
    else
    { 
      servo2_pos = servo2_pos - 20; 
      myservo2.write ( servo2_pos ); 
      delay (100); 
    } 
  } 
  if (pos_y > 700)
  {
    if (servo2_pos > 180)
    {  
    }        
    else
    {
    servo2_pos = servo2_pos + 20;
    myservo2.write(servo2_pos) ;
    delay (100) ;
    }
  }
  if (buttonState == HIGH) {
    // turn LED on:
    myservo6.write(0) ;
  } else {
    // turn LED off:
    myservo6.write(180) ;
  }
}

