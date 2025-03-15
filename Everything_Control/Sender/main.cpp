#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 to 0x3F if needed

RF24 radio(9, 10); // CE, CSN

const byte address[6] = "00001"; // Communication address
void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_HIGH);
    radio.stopListening();

    lcd.setCursor(0, 0);
    lcd.print("NRF24L01 Ready");
    delay(2000);
    lcd.clear();
}

void loop() {
    const char message[] = "Hello ESP32";
    bool sent = radio.write(&message, sizeof(message));
    //Serial.println("Sent: Hello ESP32");

     // Display sent message on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sent:");
    lcd.setCursor(0, 1);
    lcd.print(sent ? message : "Signal Not Found");

    Serial.println(sent ? "Recieved" : "Signal Not Found");
    delay(1000);
}
