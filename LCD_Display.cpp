#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Format -> (Address,Width,Height )

char *state[3] = {"1-2", "3-4", "5-6"};

void setup()
{
	// initialize the lcd
	lcd.init();
	// Turn on the Backlight
	lcd.backlight();
}

void loop()
{
	// Clear the display buffer
	lcd.clear(); 
	// Set cursor (Column, Row)
	lcd.setCursor(0, 0);
  for(int i = 0; i < 3; i++){
    lcd.print(state[i]);
    lcd.clear(); 
    delay(500);
  }
	delay(100);
}
