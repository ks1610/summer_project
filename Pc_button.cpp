#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Hien-Khanh-Ha_plus";
const char* password = "khanhha@123";
const int relayPin = 17; // Change to the pin your relay is connected to

WebServer server(80);

void handleRoot() {
    String html = "<html><head><title>ESP32 Relay Control</title></head><body>";
    html += "<h2>ESP32 Web Server - Relay Control</h2>";
    html += "<button onclick=\"location.href='/on'\">Turn ON</button>";
    html += "<button onclick=\"location.href='/off'\">Turn OFF</button>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleOn() {
    digitalWrite(relayPin, LOW);  // Activate relay (simulate button press)
    delay(500);                   // Hold for 500ms
    digitalWrite(relayPin, HIGH); // Release button
    server.send(200, "text/html", "PC Power Button Pressed! <a href='/'>Back</a>");
}


void handleOff() {
    digitalWrite(relayPin, HIGH); // Assuming HIGH turns the relay OFF
    server.send(200, "text/html", "Relay is OFF. <a href='/'>Back</a>");
}

void setup() {
    Serial.begin(115200);
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH); // Start with relay OFF
    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected!");
    Serial.println(WiFi.localIP());
    
    server.on("/", handleRoot);
    server.on("/on", handleOn);
    server.on("/off", handleOff);
    
    server.begin();
    Serial.println("Web server started!");
}

void loop() {
    server.handleClient();
}
