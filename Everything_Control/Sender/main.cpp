#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Initialize LCD (Address 0x27, change to 0x3F if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// NRF24L01 setup (Transmitter)
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001"; // Unique address for communication

// Keypad pins
const int rowPins[4] = {2, 3, 4, 5}; // R1-R4
const int colPins[4] = {A0, A1, A2, A3}; // C1-C4
const int led = 6; // LED pin

// Key mapping
char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void setup() {
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Press Key:");

    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH); // LED ON when idle

    // Initialize NRF24L01
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);
    radio.stopListening(); // Set as transmitter

    // Set row pins as OUTPUT and set HIGH
    for (int i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
    }

    // Set column pins as INPUT with PULLUP
    for (int i = 0; i < 4; i++) {
        pinMode(colPins[i], INPUT_PULLUP);
    }
}

void loop() {
    for (int row = 0; row < 4; row++) {
        digitalWrite(rowPins[row], LOW); // Activate row

        for (int col = 0; col < 4; col++) {
            if (digitalRead(colPins[col]) == LOW) { // Key pressed
                char key = keys[row][col];

                // Display on LCD
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Key Pressed:");
                lcd.setCursor(12, 0);
                lcd.print(key);

                // Turn LED OFF when a key is pressed
                digitalWrite(led, LOW);

                // Send key via NRF24L01
                radio.write(&key, sizeof(key));

                // Simple debounce
                delay(300);

                // Wait for key release
                while (digitalRead(colPins[col]) == LOW);
            }
        }

        digitalWrite(rowPins[row], HIGH); // Deactivate row
    }

    // Turn LED ON when idle
    digitalWrite(led, HIGH);
}
