#include <Servo.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Format -> (Address, Width, Height)

const int buttonPin = 12;  // Pushbutton pin
int buttonState = 0;       // Current button state
int lastButtonState = 0;   // Previous button state to detect changes

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

int count = 0;

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

  // Initialize the pushbutton pin with internal pull-up resistor:
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Initialize the LCD:
  lcd.init();
  lcd.backlight();
  
  // Display initial state as "NOT Pressed" (since pull-up means default HIGH):
  lcd.setCursor(0, 0);
  lcd.print("NOT Pressed");
}

int button_count(){
  // Read the current state of the pushbutton:
  buttonState = digitalRead(buttonPin);
  
  // Check if the button state has changed:
  if (buttonState != lastButtonState) {
    // Debounce delay
    delay(50);
    
    // Re-read the button state after debounce delay:
    buttonState = digitalRead(buttonPin);
    
    // If the button is pressed (LOW due to pull-up resistor):
    if (buttonState == LOW) {
      lcd.clear();
      lcd.setCursor(0, 0);
      count++;
      lcd.print(count);
      delay(500);
    } 
    // If the button is released (HIGH due to pull-up):
    else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Current State");
      lcd.setCursor(0, 1);
      lcd.print(count);
    }
    if(count == 3)
      count = 0;
  }
  
  // Update lastButtonState to the current state:
  lastButtonState = buttonState;

  return count;
}


void DOF_1_2(){
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
}

void DOF_3_4(){
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
      myservo3.write ( servo1_pos ) ; 
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
    myservo3.write ( servo1_pos ) ;
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
      myservo4.write ( servo2_pos ); 
      delay (30); 
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
    myservo4.write(servo2_pos) ;
    delay (30) ;
    }
  }
}

void DOF_5_6(){
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
      myservo5.write ( servo1_pos ) ; 
      delay (30); 
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
    myservo5.write ( servo1_pos ) ;
    delay (30) ;
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
      myservo6.write ( servo2_pos ); 
      delay (30); 
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
    myservo6.write(servo2_pos) ;
    delay (30) ;
    }
  }
}
void loop() {
  int state = button_count();
  
  switch(state){
    case 0:
      DOF_1_2();
      break;
    case 1:
      DOF_3_4();
      break;
    case 2:
      DOF_5_6();
      break;
  } 
}

