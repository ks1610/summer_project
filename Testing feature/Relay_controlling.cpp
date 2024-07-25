// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "time.h" // Library to get current time

// Set to true to define Relay as Normally Open (NO)
#define RELAY_NO    true

// Set number of relays
#define NUM_RELAYS  2

// Assign each GPIO to a relay
int relayGPIOs[NUM_RELAYS] = {26, 25};

// Replace with your network credentials
const char* ssid = "Hien-Khanh-Ha_plus";
const char* password = "khanhha@123";

const char* PARAM_INPUT_1 = "relay";  
const char* PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String relayState(int numRelay){
  if(RELAY_NO){
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "";
    }
    else {
      return "checked";
    }
  }
  else {
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "checked";
    }
    else {
      return "";
    }
  }
  return "";
}

String relayState25() {
  if(RELAY_NO) {
    return digitalRead(25) ? "off" : "on";
  } else {
    return digitalRead(25) ? "on" : "off";
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    .button {
      display: inline-block;
      padding: 10px 10px;
      font-size: 20px;
      cursor: pointer;
      text-align: center;
      text-decoration: none;
      outline: none;
      color: #fff;
      background-color: #4c814e;
      border: none;
      border-radius: 15px;
      width: 200px;
      margin-right: 10px;
    }
    .button:hover {background-color: #3e8e41}
    .button:active {
      background-color: #4c814e;
      transform: translateY(4px);
      padding: 20px 10px;
    }
    .button.grey {background-color: #4c814e;}
    .button.green {background-color: #7a7979e1;}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
<script>
function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
  xhr.send();
}

function toggleButton() {
  var xhttp = new XMLHttpRequest();
  var button = document.getElementById("relayButton");
  if (button.classList.contains("grey")) {
    button.classList.remove("grey");
    button.classList.add("green");
    button.innerHTML = "Thank you &#x1F60A";
    xhttp.open("GET", "/update?relay=2&state=1", true);
  } else {
    button.classList.remove("green");
    button.classList.add("grey");
    button.innerHTML = "Give me some water, please &#128519;";
    xhttp.open("GET", "/update?relay=2&state=0", true);
  }
  xhttp.send();
  setTimeout(function() {
    if (button.innerHTML == "Thank you &#x1F60A") {
      button.innerHTML = "Give me some water, please &#128519;";
    }
  }, 6000);
}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    buttons += "<label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"1\" "+ relayState(1) +"><span class=\"slider\"></span></label>";
    String relayStateValue = relayState25();
    String buttonClass = (relayStateValue == "off") ? "grey" : "green";
    String buttonText = (relayStateValue == "off") ? "Give me some water, please &#128519;" : "Thank you &#x1F60A";
    buttons += "<button id=\"relayButton\" class=\"button " + buttonClass + "\" onclick=\"toggleButton()\">" + buttonText + "</button>";
    return buttons;
  }
  return String();
}

// Function to get the current day of the week
int getCurrentDayOfWeek() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return -1;
  }
  return timeinfo.tm_wday; // 0 = Sunday, 1 = Monday, ..., 6 = Saturday
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Set all relays to off when the program starts - if set to Normally Open (NO), the relay is off when you set the relay to HIGH
  for(int i=0; i<NUM_RELAYS; i++){
    pinMode(relayGPIOs[i], OUTPUT);
    if(RELAY_NO){
      digitalWrite(relayGPIOs[i], HIGH); // Turn off all relays
    }
    else{
      digitalWrite(relayGPIOs[i], LOW); // Turn off all relays
    }
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Initialize NTP
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?relay=<inputMessage>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputMessage2;
    String inputParam2;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) & request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      if(RELAY_NO){
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], !inputMessage2.toInt());
      }
      else{
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], inputMessage2.toInt());
      }
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage + inputMessage2);
    request->send(200, "text/plain", "OK");
  });
  // Start server
  server.begin();
}

void loop() {
  int currentDayOfWeek = getCurrentDayOfWeek();
  if (currentDayOfWeek == 1 || currentDayOfWeek == 3 || currentDayOfWeek == 5) {
    digitalWrite(relayGPIOs[0], LOW); // Turn on relay connected to GPIO 26
  } else {
    digitalWrite(relayGPIOs[0], HIGH); // Turn off relay connected to GPIO 26
  }
  delay(1000); // Delay to avoid rapid looping
}
