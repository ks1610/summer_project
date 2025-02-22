#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Hien-Khanh-Ha_plus";
const char* password = "khanhha@123";
const int relayPin = 17;

WebServer server(80);

void handleRoot() {
    String html = "<html><body>";
    html += "<h1>ESP32 Web Server</h1>";
    html += "<p><button onmousedown=\"fetch('/on')\" onmouseup=\"fetch('/off')\">Press and Hold</button></p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleOn() {
    digitalWrite(relayPin, HIGH);
    server.send(200, "text/plain", "Relay ON");
}

void handleOff() {
    digitalWrite(relayPin, LOW);
    server.send(200, "text/plain", "Relay OFF");
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
    Serial.println(WiFi.localIP());
    
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);

    server.on("/", handleRoot);
    server.on("/on", handleOn);
    server.on("/off", handleOff);
    
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}
