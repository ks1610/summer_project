#include <Wire.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// NRF24L01 setup
RF24 radio(2, 4); // CE, CSN
const byte address[6] = "00001";

// Relay pins
const int relayPins[4] = {26, 25, 33, 32};

void setup() {
  Serial.begin(115200);
  Serial.println("Booting ESP32 Receiver...");

  Wire.begin(21, 22); // I2C for OLED

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ OLED init failed!");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  if (!radio.begin()) {
    Serial.println("❌ NRF24L01 init failed!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("NRF24L01 Failed");
    display.display();
    while (true);
  } else {
    Serial.println("✅ NRF24L01 OK");
  }

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  // Setup relay pins
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW); // Relays off by default
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("NRF24L01 Ready");
  display.setCursor(0, 20);
  display.println("Waiting...");
  display.display();
}

void loop() {
  if (radio.available()) {
    char receivedChar;
    radio.read(&receivedChar, sizeof(receivedChar));

    Serial.println("✅ Data received!");
    Serial.print("Character: ");
    Serial.println(receivedChar);

    // Show on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("Received:");
    display.setCursor(0, 20);
    display.setTextSize(2);
    display.print(receivedChar);
    display.display();

    // Control relays based on key
    switch (receivedChar) {
      case '1':
        digitalWrite(relayPins[0], HIGH);
        break;
      case '2':
        digitalWrite(relayPins[1], HIGH);
        break;
      case '3':
        digitalWrite(relayPins[2], HIGH);
        break;
      case '4':
        digitalWrite(relayPins[3], HIGH);
        break;
      case 'A':
        // Turn all off
        for (int i = 0; i < 4; i++) {
          digitalWrite(relayPins[i], LOW);
        }
        break;
    }
  } else {
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 1000) {
      Serial.println("🔄 Waiting for data...");
      lastCheck = millis();
    }
  }
}
