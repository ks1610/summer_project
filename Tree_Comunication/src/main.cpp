#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "Hien-Khanh-Ha_plus";
const char* password = "khanhha@123";

#define DHTPIN 27     // Digital pin connected to the DHT sensor
#define SOIL_MOISTURE_PIN 34  // Analog pin connected to the soil moisture sensor
const int relay = 26; // Digital pin connected to Relay

#define RELAY_NO true

#define DHTTYPE DHT11 // DHT 11
// Uncomment the type of sensor in use:
// #define DHTTYPE DHT22 // DHT 22 (AM2302)
// #define DHTTYPE DHT21 // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

const char* PARAM_INPUT_1 = "relay";
const char* PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  } else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  } else {
    Serial.println(h);
    return String(h);
  }
}

String readSoilMoisture() {
  double data = analogRead(SOIL_MOISTURE_PIN);
  data = 100 - ((data / 4095.0) * 100);
  if (isnan(data)) {
    Serial.println("Failed to read from Soil Moisture sensor!");
    return "--";
  } else {
    Serial.print("Soil Moisture: ");
    Serial.println(data);
    return String(data) + "%";
  }
}

String relayState() {
  if (RELAY_NO) {
    return digitalRead(relay) ? "stop" : "water";
  } else {
    return digitalRead(relay) ? "water" : "stop";
  }
}

String getMoodStatus() {
  double soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  soilMoisture = 100 - ((soilMoisture / 4095.0) * 100);
  if (soilMoisture <= 10) {
    return "<span class='thirsty'>I'm so thirsty, right now &#128560;</span>";
  } 
  else if (soilMoisture > 10 && soilMoisture < 60) {
    return "<span class='normal'>This air is good for me now &#128521;</span>";
  } 
  else {
    return "<span class='good'>I'm good right now &#x1F60A;</span>";
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
   @import url('https://fonts.googleapis.com/css2?family=Roboto:wght@400&display=swap');
    html {
      font-family: 'Roboto', sans-serif;
      display: inline-block;
      margin: 0px auto;
      text-align: center;
      overflow-x: hidden; /* Prevent horizontal overflow */
    }
    h2 { 
      font-size: 3.0rem; 
    }
    p { 
      font-size: 3.0rem;     
      margin-bottom: 10px;
      margin-top: 0px;
      font-family: 'Roboto', sans-serif;
    }    
    body {
      height: 100vh;
      margin: 0;
      font-family: 'Roboto', sans-serif;
      display: flex;
      justify-content: center;
      align-items: center;
      overflow-x: hidden; /* Prevent horizontal overflow */
    }
    .page_container {
      width: 70%;
      display: flex;
      justify-content: center;
      align-items: center;
      flex-direction: column;
      margin: auto;
      height: 100vh;
      overflow: auto; /* Allow scrolling if content overflows */
    }
    .status_icon img {
      max-width: 100%;
      max-height: 100%;
      object-fit: contain;
    }
    .units { 
      font-size: 1.2rem; 
    }
    .dht-labels, .soil-labels {
      font-size: 1.5rem;
      vertical-align: middle;
    }
    .mood_status {
      font-size: 1.7rem;
    }
    .Soild_sensor,
    .DHT11_sensor,
     {
      border-radius: 25px;
      background-color: black;
      color: white;
      margin: 10px 0;
      width: 70%;
      padding: 10px; /* Added padding */
      overflow: hidden; /* Ensure content doesn't overflow */
    }
    .thirsty {
      color: red;
      padding: 10px;
      border-radius: 5px;
    }
    .good {
      color: blue;
      padding: 10px;
      border-radius: 5px;
    }
    .normal{
      color: white;
      padding: 10px;
      border-radius: 5px;
    }
    @media screen and (max-width: 767px) {
      .Soild_sensor,
      .DHT11_sensor,
      .mood_status{
        border-radius: 25px;
        background-color: black;
        color: white;
        margin: 10px;
        width: 90%;
        overflow: hidden; /* Ensure content doesn't overflow */
      }
      .page_container {
        width: 90%;
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: column;
        margin: auto;
        overflow: auto; /* Allow scrolling if content overflows */
      }
      .soil-labels,
      .dht-labels,
      .mood_status {
        font-size: 20px;
      }
      .dht-labels {
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: column;
      }
      .humidi,
      .temp,
      .Soild_sensor{
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: row;
      }
    }
    #temperature, #humidity, #data {
      font-size: 1.5rem;
    }
    .button {
      display: inline-block;
      padding: 10px 20px;
      font-size: 20px;
      cursor: pointer;
      text-align: center;
      text-decoration: none;
      outline: none;
      color: #fff;
      background-color: #4CAF50;
      border: none;
      border-radius: 15px;
      width: 250px;
    }
    .button:hover {background-color: #3e8e41}
    .button:active {
      background-color: #3e8e41;
      transform: translateY(4px);
    }
    .button.grey {background-color: #4CAF50;}
    .button.green {background-color: #b0b0b0;}
  </style>
</head>
<body>
  <div class="page_container">
    <div style = "margin-bottom: 10px;">
      <img class="status_icon" style = "width: 200px; height: 200px;"
        src="https://cdn-icons-png.flaticon.com/512/483/483683.png" alt="happy_status">
    </div>
    <!--p class="mood_status" id="mood_data" style = "margin-bottom: 10px; 
    padding: 10px 5px; 
    font-size: 20px;">Hi&#128075;, </p-->
    
    <p class="mood_status" id="mood_data" style = "margin-bottom: 10px; 
    padding: 10px 5px; 
    font-size: 20px;">%MOODPLACEHOLDER%</p>
    
    <div class="DHT11_sensor" style = "margin-bottom: 10px;
    padding: 10px 5px;">
      <p class = "temp">
        <span class="dht-labels">&#127777; Temperature: </span> 
        <span id="temperature"> %TEMPERATURE% <!--sup>&deg;C</sup--></span>
        <sup class="units" style = "font-size: 18px;">&deg;C</sup>
      </p>
      <p class = "humidi"> 
        <span class="dht-labels">&#128167; Humidity: </span>
        <span id="humidity"> %HUMIDITY% <!--sup>&percnt;</sup--></span>
        <sup class="units" style = "font-size: 18px;">&percnt;</sup>
      </p>
    </div>
    <p class="Soild_sensor" style = "margin-bottom: 10px;
    padding: 10px 5px;">
      <span class="soil-labels"><a>&#127792;</a>Soil Moisture: </span>
      <span id="data"></span>
    </p>
    %BUTTONPLACEHOLDER%
  </div>
</body>
<script>
setInterval(function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000);

setInterval(function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000);

setInterval(function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("data").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/soil-moisture", true);
  xhttp.send();
}, 10000);

setInterval(function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("mood_data").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/mood", true);
  xhttp.send();
}, 10000);

function toggleButton() {
  var xhttp = new XMLHttpRequest();
  var button = document.getElementById("relayButton");
  if (button.classList.contains("grey")) {
    button.classList.remove("grey");
    button.classList.add("green");
    button.innerHTML = "Thank you &#x1F60A";
    xhttp.open("GET", "/update?relay=relay&state=1", true);
  } else {
    button.classList.remove("green");
    button.classList.add("grey");
    button.innerHTML = "Give me some water, please &#128519;";
    xhttp.open("GET", "/update?relay=relay&state=0", true);
  }
  xhttp.send();
  setTimeout(function() {
    if (button.innerHTML == "Thank you &#x1F60A") {
      button.innerHTML = "Give me some water, please &#128519;";
    }/* else {
      button.innerHTML = "OK &#128076;";
    }*/
  }, 6000);
}
</script>
</html>)rawliteral";


// Replaces placeholder with DHT values
String processor(const String& var) {
  if (var == "TEMPERATURE") {
    return readDHTTemperature();
  } else if (var == "HUMIDITY") {
    return readDHTHumidity();
  } else if (var == "BUTTONPLACEHOLDER") {
    String relayStateValue = relayState();
    //String buttonClass = (relayStateValue == "stop") ? "green" : "grey";
    String buttonClass = (relayStateValue == "stop") ? "grey" : "green";
    //String buttonText = (relayStateValue == "stop") ? "Stop" : "Water";
    String buttonText = (relayStateValue == "stop") ? "Give me some water, please &#128519;" : "OK &#128076;";
    String buttons = "<button id=\"relayButton\" class=\"button " + buttonClass + "\" onclick=\"toggleButton()\">" + buttonText + "</button>";
    return buttons;
  } else if (var == "SOILMOISTURE") {
    return readSoilMoisture();
  } else if (var == "MOODPLACEHOLDER") {
    return getMoodStatus();
  }
  return String();
}

void setup() {
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  Serial.begin(115200);

  dht.begin();

  pinMode(relay, OUTPUT);
  if (RELAY_NO) {
    digitalWrite(relay, HIGH);
  } else {
    digitalWrite(relay, LOW);
  }
/*
  // Set up the ESP32 as an Access Point
  WiFi.softAP(ssid, password);

  // Print ESP32 Local IP Address
  Serial.println(WiFi.softAPIP());
*/
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
  server.on("/soil-moisture", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readSoilMoisture().c_str());
  });

  // Route to get mood status
  server.on("/mood", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", getMoodStatus().c_str());
  });
  
  // Send a GET request to <ESP_IP>/update?relay=<inputMessage>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputMessage2;
    String inputParam;
    String inputParam2;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      if (RELAY_NO) {
        digitalWrite(relay, !inputMessage2.toInt());
      } else {
        digitalWrite(relay, inputMessage2.toInt());
      }
    } else {
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
  // Nothing needed here, handled by server
}
