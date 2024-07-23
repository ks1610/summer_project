

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "Hien-Khanh-Ha_plus";
const char* password = "khanhha@123";

#define DHTPIN 27     // Digital pin connected to the DHT sensor
#define SOIL_MOISTURE_PIN 34  // Analog pin connected to the soil moisture sensor
/*const int relay = 26; // Digital pin connected to Relay
const int relay1 = 25; // Digital pin connected to Relay
//const int relay1 = 35; // Digital pin connected to Relay*/


// Set to true to define Relay as Normally Open (NO)
#define RELAY_NO    true

// Set number of relays
#define NUM_RELAYS  2

// Assign each GPIO to a relay
int relayGPIOs[NUM_RELAYS] = {25, 26};

#define DHTTYPE DHT11 // DHT 11
// Uncomment the type of sensor in use:
// #define DHTTYPE DHT22 // DHT 22 (AM2302)
// #define DHTTYPE DHT21 // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

const char* PARAM_INPUT_1 = "relay";  
const char* PARAM_INPUT_2 = "state";

/*const char* PARAM_INPUT_1_1 = "relay1";
const char* PARAM_INPUT_2_1 = "state1";*/

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

/*String relayState() {
  if (RELAY_NO) {
    return digitalRead(relay) ? "stop" : "water";
  } else {
    return digitalRead(relay) ? "water" : "stop";
  }
}

String relayState1() {
  if (RELAY_NO1) {
    return digitalRead(relay1) ? "fan off" : "fan on";
  } else {
    return digitalRead(relay1) ? "fan on" : "fan off";
  }
}*/

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
      margin: 0;
      font-family: 'Roboto', sans-serif;
      display: flex;
      justify-content: center;
      align-items: center;
      overflow-x: hidden; /* Prevent horizontal overflow */
    }
    /*body {
        background-image: url('https://images.unsplash.com/photo-1517928260182-5688aead3066?fm=jpg&q=60&w=3000&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxzZWFyY2h8NHx8YXVyb3JhfGVufDB8fDB8fHww'); 
        -webkit-background-size: cover;
        -moz-background-size: cover;
        -o-background-size: cover;
        background-repeat: no-repeat;
        background-size: cover;
        height: 100vh;
    }*/
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
    .DHT11_sensor{
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
        overflow: hidden; /* Ensure content doesn't overflow */
        width: 90%;
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
        flex-direction: column;
      }
      
      .Relay_State{
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: row-reverse;
        margin: 10px;
      }

      .Temp_Soil_Sensor{
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

    #datetime {
        font-size: 25px;
        font-family: 'Roboto', sans-serif;
        color: white;
    }
    .Date_Time_status{
      background-color: black;
      border-radius: 25px;
        color: white;
        padding: 10px;
        text-align: center;
        margin-bottom: 10px;
    }
    .detail{
        font-size: 10px;
        border-radius: 25px;
        background-color: black;
        color: white;
        text-align: center;
        margin-bottom: 10px;
    }
    .morning{
        font-size: 20px;
        color: aqua;
        text-align: center;
        font-family: 'Roboto', sans-serif;
        margin-bottom: 0px;
    }
    .afternoon{
        font-size: 20px;
        color:bisque;
        text-align: center;
        font-family: 'Roboto', sans-serif;
        margin-bottom: 0px;
    }
    .evening{
        font-size: 20px;
        color: cadetblue;
        text-align: center;
        font-family: 'Roboto', sans-serif;
        margin-bottom: 0px;
    }
    body.morning-bg {
        background-image: url('https://w0.peakpx.com/wallpaper/60/636/HD-wallpaper-tree-green-natural-nature-trees-thumbnail.jpg');
        -webkit-background-size: cover;
        -moz-background-size: cover;
        -o-background-size: cover;
        background-repeat: no-repeat;
        background-size: cover;
        height: 100vh;
    }
    body.afternoon-bg {
        background-image: url('https://i.pinimg.com/736x/87/6e/fc/876efc60c78b1c936d7a2527589320e8.jpg');
        -webkit-background-size: cover;
        -moz-background-size: cover;
        -o-background-size: cover;
        background-repeat: no-repeat;
        background-size: cover;
        height: 100vh;
    }
    body.evening-bg {
        background-image: url('https://images.unsplash.com/photo-1517928260182-5688aead3066?fm=jpg&q=60&w=3000&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxzZWFyY2h8NHx8YXVyb3JhfGVufDB8fDB8fHww');
        -webkit-background-size: cover;
        -moz-background-size: cover;
        -o-background-size: cover;
        background-repeat: no-repeat;
        background-size: cover;
        height: 100vh;
    }
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 25px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 25px}
    input:checked+.slider {background-color: #83e0aae8}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}

  </style>
</head>
<body>
  <div class="page_container">
     <!--display time-->
    <div style = "margin-bottom: 10px;">
      <img class="status_icon" style = "width: 200px; height: 200px;"
        src="https://cdn-icons-png.flaticon.com/512/483/483683.png" alt="happy_status">
    </div>

  <div class = "Date_Time_status">
    <p class="detail"></p><!--display day status-->
    <span id="datetime"></span>
  </div>
    <!--p class="mood_status" id="mood_data" style = "margin-bottom: 10px; 
    padding: 10px 5px; 
    font-size: 20px;">Hi&#128075;, </p-->
    
    <p class="mood_status" id="mood_data" style = "margin-bottom: 10px; 
    padding: 10px 5px; 
    font-size: 20px;">%MOODPLACEHOLDER%</p>
    <div class = "Temp_Soil_Sensor">
      <div class="DHT11_sensor" style = "margin-bottom: 10px;
        padding: 10px 5px;">
        <p class = "temp">
          <span class="dht-labels">&#127777; Temperature: </span>
          <div> 
            <span id="temperature"> %TEMPERATURE% <!--sup>&deg;C</sup--></span>
            <sup class="units" style = "font-size: 18px;">&deg;C</sup>
          </div>
        </p>
        <p class = "humidi">
          <span class="dht-labels">&#128167; Humidity: </span>
          <div>
            <span id="humidity"> %HUMIDITY% <!--sup>&percnt;</sup--></span>
            <sup class="units" style = "font-size: 18px;">&percnt;</sup>
          </div>
        </p>
      </div>
      <div class = soil>
        <p class="Soild_sensor" style = "margin-bottom: 10px;
          padding: 10px 5px;
          height: 145px;">
          <span class="soil-labels"><a>&#127792;</a>Soil Moisture: </span>
          <span id="data">--</span>
        </p>
      </div>
    </div>
    <div class = "Relay_State">
      %BUTTONPLACEHOLDER%
    </div>
  </div>
</body>
<script>

// Create a function to update the time
function updateTime() {
  // Create a new `Date` object
  const now = new Date();
  // Get the current hours, minutes, and seconds
  const hours = now.getHours().toString().padStart(2, '0');
  const minutes = now.getMinutes().toString().padStart(2, '0');
  const seconds = now.getSeconds().toString().padStart(2, '0');
  // Format the time as HH:MM:SS
  const currentTime = `${hours}:${minutes}:${seconds}`;
  // Display the current time in the span
  document.getElementById('datetime').innerText = currentTime;
  
  // Log the current time to the console
  console.log(currentTime);
  if (currentTime >= "00:00:00" && currentTime <= "11:59:59") {
    document.getElementsByClassName("detail")[0].innerHTML = `<p class="morning">Hi, Good morning &#127774;</p>`;
    document.body.className = "morning-bg"; // Reset to default background
  }
  else if (currentTime >= "12:00:00" && currentTime <= "17:59:59") {
    document.getElementsByClassName("detail")[0].innerHTML = `<p class="afternoon">Hi, Good afternoon &#127773;</p>`;
    document.body.className = "afternoon-bg"; // Reset to default background
  }else if (currentTime >= "18:00:00" && currentTime <= "23:59:59") {
    document.getElementsByClassName("detail")[0].innerHTML = `<p class="evening">Hi, Good evening &#127770;</p>`;
    document.body.className = "evening-bg"; // Set evening background
  }
}
// Call the function immediately to display the time right away
updateTime();
// Update the time every second
setInterval(updateTime, 1000);

//create function to get temperature
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

//create function to get humidity
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

//create function to get soil moisture
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

//create function to get mood status
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

/*
//create function to toggle Relay 
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
/*  }, 6000);
}

//added new function for Fan relay
function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay1="+element.id+"&state1=1", true); }
  else { xhr.open("GET", "/update?relay1="+element.id+"&state1=0", true); }
  xhr.send();
}
*/


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
</html>)rawliteral";


// Replaces placeholder with DHT values
String processor(const String& var) {
  if (var == "TEMPERATURE") {
    return readDHTTemperature();
  } 
  else if (var == "HUMIDITY") {
    return readDHTHumidity();
  } 
  /*else if (var == "BUTTONPLACEHOLDER") {
    String relayStateValue = relayState();
    //String buttonClass = (relayStateValue == "stop") ? "green" : "grey";
    String buttonClass = (relayStateValue == "stop") ? "grey" : "green";
    //String buttonText = (relayStateValue == "stop") ? "Stop" : "Water";
    String buttonText = (relayStateValue == "stop") ? "Give me some water, please &#128519;" : "OK &#128076;";
    String buttons = "<button id=\"relayButton\" class=\"button " + buttonClass + "\" onclick=\"toggleButton()\">" + buttonText + "</button>";
    return buttons;
  }
  else if(var == "BUTTONPLACEHOLDER1"){
    String buttons ="";
    String relayStateValue = relayState1();
    buttons+= "<label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String() + "\" "+ relayStateValue +"><span class=\"slider\"></span></label>";
    return buttons;
  }*/

  else if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    buttons += "<label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"1\" "+ relayState(1) +"><span class=\"slider\"></span></label>";
    String relayStateValue = relayState25();
    String buttonClass = (relayStateValue == "off") ? "grey" : "green";
    String buttonText = (relayStateValue == "off") ? "Give me some water, please &#128519;" : "Thank you &#x1F60A";
    buttons += "<button id=\"relayButton\" class=\"button " + buttonClass + "\" onclick=\"toggleButton()\">" + buttonText + "</button>";
    return buttons;
  }

  else if (var == "SOILMOISTURE") {
    return readSoilMoisture();
  } 
  else if (var == "MOODPLACEHOLDER") {
    return getMoodStatus();
  }
  return String();
}

void setup() {
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  Serial.begin(115200);

  dht.begin();

  /*
  pinMode(relay, OUTPUT);
  if (RELAY_NO) {
    digitalWrite(relay, HIGH);
  } else {
    digitalWrite(relay, LOW);
  }

  pinMode(relay1, OUTPUT);
  if (RELAY_NO1) {
    digitalWrite(relay1, HIGH);
  } else {
    digitalWrite(relay1, LOW);
  }
  */

  for(int i=0; i<NUM_RELAYS; i++){
    pinMode(relayGPIOs[i], OUTPUT);
    if(RELAY_NO){
      digitalWrite(relayGPIOs[i], HIGH);
    }
    else{
      digitalWrite(relayGPIOs[i], LOW);
    }
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
  
  /*
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
  
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputMessage2;
    String inputParam2;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1_1) & request->hasParam(PARAM_INPUT_2_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1_1)->value();
      inputParam = PARAM_INPUT_1_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2_1)->value();
      inputParam2 = PARAM_INPUT_2_1;
      if(RELAY_NO1){
        Serial.print("NO ");
        digitalWrite(relay1, !inputMessage2.toInt());
      }
      else{
        Serial.print("NC ");
        digitalWrite(relay1, inputMessage2.toInt());
      }
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage + inputMessage2);
    request->send(200, "text/plain", "OK");
  });*/

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
  // Nothing needed here, handled by server
}
