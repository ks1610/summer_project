#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Format -> (Address, Width, Height)

const int buttonPin = 12;  // Pushbutton pin

int buttonState = 0;       // Current button state
int lastButtonState = 0;   // Previous button state to detect changes

int count = 0;

void setup() {
  // Initialize the pushbutton pin with internal pull-up resistor:
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Initialize the LCD:
  lcd.init();
  lcd.backlight();
  
  // Display initial state as "NOT Pressed" (since pull-up means default HIGH):
  lcd.setCursor(0, 0);
  lcd.print("NOT Pressed");
}

void loop() {
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
      lcd.print(count+1);
    } 
    // If the button is released (HIGH due to pull-up):
    else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("NOT Pressed");
      lcd.setCursor(0, 1);
      lcd.print(count);
    }
    if(count == 3)
      count = 0;
  }

  // Update lastButtonState to the current state:
  lastButtonState = buttonState;
}
