#include <Wire.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RF24 radio(4, 5); // CE, CSN
const byte address[6] = "00001"; // Same as sender

void setup() {
  Serial.begin(115200);
  Serial.println("Starting NRF24L01 Receiver...");

  Wire.begin(21, 22);  // ESP32 I2C pins (SDA=21, SCL=22)

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // OLED I2C address
      Serial.println("SSD1306 allocation failed");
      for (;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("NRF24L01 Ready");
  display.display();
  delay(2000);
  display.clearDisplay();

  if (!radio.begin()) {
      Serial.println("NRF24L01 module not detected ❌");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("NRF24L01 ERROR!");
      display.display();
      while (1); // Stop execution
  } else {
      Serial.println("NRF24L01 detected ✅");
  }

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);  // Use RF24_PA_MIN for initial testing
  radio.startListening();
}

void loop() {
    if (radio.available()) {
        char message[32] = "";
        radio.read(&message, sizeof(message));

        // Display received message on OLED
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(1);
        display.print("Received:");
        display.setCursor(0, 20);
        display.setTextSize(1);
        display.print(message);
        display.display();

        Serial.print("Received: ");
        Serial.println(message);
    }
}
