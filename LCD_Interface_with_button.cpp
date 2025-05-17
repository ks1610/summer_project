#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Format: (Address, Columns, Rows)

const int buttonPin[] = {29, 30, 31, 32};  // Pushbutton pins
const int buttonNum = sizeof(buttonPin) / sizeof(buttonPin[0]);

void setup() {
  Serial.begin(9600);

  // Initialize button pins with pull-up
  for (int i = 0; i < buttonNum; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("NOT Pressed");
}

void loop() {
  for (int i = 0; i < buttonNum; i++) {
    int state = digitalRead(buttonPin[i]);

    // Buttons are active LOW (because of INPUT_PULLUP)
    if (state == LOW) {
      lcd.clear();  // Clear previous text
      lcd.setCursor(0, 0);
      switch (i) {
        case 0:
          lcd.print("Feature 1");
          //do function 1
          break;
        case 1:
          lcd.print("Feature 2");
          //do function 2
          break;
        case 2:
          lcd.print("Feature 3");
          //do function 3
          break;
        case 3:
          lcd.print("Feature 4");
          //do function 4
          break;
      }
      delay(300); // Debounce delay
    }
  }
}
