/* ----------------------------------------------------------- */
/*                           LIBRARY                           */
/* ----------------------------------------------------------- */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
/* ----------------------------------------------------------- */
/*                            DEFINE                           */
/* ----------------------------------------------------------- */
//// Oled ////
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //
#define TIME_VIEW 3000      // Unit (ms)
//// Wifi ////
#define SSID_WIFI "Hien-Khanh-Ha_plus" //"REPLACE_WITH_YOUR_SSID"
#define PASS_WIFI "khanhha@123" //"REPLACE_WITH_YOUR_PASSWORD"
//// OpenWeather ////
#define LATITUDE  "21.02" //"YOUR_LATITUDE"   // Ex: "10.77"
#define LONGITUDE "105.84" //"YOUR_LONGITUDE" // Ex: "106.65"
#define API_KEY   "91de37762ab7c52e4866abb97f0fcc82" //"REPLACE_WITH_YOUR_OPEN_WEATHER_MAP_API_KEY"
//
// Check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
#define TIME_UPDATE 10000UL // Unit (ms)
//// API "Current weather data" ////
#define NAME_URL "http://api.openweathermap.org/data/2.5/weather"
#define PARAM_LAT "?lat="
#define PARAM_LON "&lon="
#define PARAM_API "&appid="
#define PARAM_UNIT "&units=metric"
//
String URL = String(NAME_URL) + String(PARAM_LAT) + String(LATITUDE) + String(PARAM_LON) + String(LONGITUDE) + String(PARAM_API) + String(API_KEY) + String(PARAM_UNIT);
/* ----------------------------------------------------------- */
/*                            OBJECT                           */
/* ----------------------------------------------------------- */
//// Oled ////
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//// JSON ////
String jsonBuffer;
JSONVar myObject;
enum WeatherContent : int
{
  WEATHER,
  TEMP,
  HUMI,
  FEEL,
  WIND,
  CLOUDS,
};
WeatherContent menu = WEATHER;
/* ----------------------------------------------------------- */
/*                            CONFIG                           */
/* ----------------------------------------------------------- */
void setup()
{
  Serial.begin(115200);
  WiFi.begin(SSID_WIFI, PASS_WIFI);
  Serial.println(F("Connecting"));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F(""));
  //
  Serial.print(F("Connected to WiFi network with IP Address: "));
  Serial.println(WiFi.localIP());
  //
  Serial.print(F("Pls, wait in about "));
  Serial.print(TIME_UPDATE / 1000);
  Serial.println(" seconds");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  //
  display.clearDisplay();              // Clear the buffer
  //display.setRotation(2);              // Rotate the screen vertically
  display.setRotation(0);               //rotates text on OLED 1=90 degrees, 2=180 degrees
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);                 // Use full 256 char 'Code Page 437' font
}
/* ----------------------------------------------------------- */
/*                             MAIN                            */
/* ----------------------------------------------------------- */
String httpGETRequest(const char *serverName);
void displayOled();
void loop()
{
  /* ---------------- Send an HTTP GET request --------------- */
  if ((millis() - lastTime) >= TIME_UPDATE)
  {
    /* ------------- Check WiFi connection status ------------ */
    if (WiFi.status() == WL_CONNECTED)
    {
      jsonBuffer = httpGETRequest(URL.c_str());
      // Serial.println(jsonBuffer);
      myObject = JSON.parse(jsonBuffer);
      // Check received JSON packet has data?
      if (JSON.typeof(myObject) == "undefined")
      {
        Serial.println(F("Parsing input failed!"));
        return;
      }
      /* ----------------------------------------------------- */
      Serial.print("Weather: ");
      Serial.print(myObject["weather"][0]["main"]);
      Serial.print(" - ");
      Serial.println(myObject["weather"][0]["description"]);
      /* ----------------------------------------------------- */
      Serial.print("Temperature: ");
      Serial.print(myObject["main"]["temp"]);
      Serial.println("°C");
      //
      Serial.print("Humidity: ");
      Serial.print(myObject["main"]["humidity"]);
      Serial.println("%");
      //
      Serial.print("Feels Like: ");
      Serial.print(myObject["main"]["feels_like"]);
      Serial.println("°C");
      /* ----------------------------------------------------- */
      Serial.print("Wind Speed: ");
      Serial.print(myObject["wind"]["speed"]);
      Serial.println("m/s");
      //
      Serial.print("Wind Direction: ");
      Serial.print(myObject["wind"]["deg"]);
      Serial.println("°");
      /* ----------------------------------------------------- */
      Serial.print("Cloudiness: ");
      Serial.print(myObject["clouds"]["all"]);
      Serial.println("%");
      /* ----------------------------------------------------- */
      Serial.println(F(""));
    }
    else
    {
      Serial.println(F("WiFi Disconnected"));
    }
    /* ------------------------------------------------------- */
    lastTime = millis();
  }
  displayOled(); // Show current weather information
}
/* ----------------------------------------------------------- */
/*                           FUNCTION                          */
/* ----------------------------------------------------------- */
String httpGETRequest(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  String payload = "{}";
  if (httpResponseCode > 0)
  {
    Serial.print(F("HTTP Response code: "));
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print(F("Error code: "));
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}
/* ----------------------------------------------------------- */
void displayOled()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  switch (menu)
  {
  case WEATHER:
  {
    display.setTextSize(2);
    display.print(myObject["weather"][0]["main"]);
    display.print(F("\n\n"));
    //
    display.setTextSize(1);
    display.print(F("\x1A "));
    display.print(myObject["weather"][0]["description"]);
  }
    menu = TEMP;
    break;
  case TEMP:
  {
    display.setTextSize(1);
    display.print(F("Temperature:"));
    display.print(F("\n\n"));
    //
    display.setTextSize(2);
    display.print(myObject["main"]["temp"]);
    display.print(F(" \xF8\x43"));
  }
    menu = HUMI;
    break;
  case HUMI:
  {
    display.setTextSize(1);
    display.print(F("Humidity:"));
    display.print(F("\n\n"));
    //
    display.setTextSize(2);
    display.print(myObject["main"]["humidity"]);
    display.print(F(" %"));
  }
    menu = FEEL;
    break;
  case FEEL:
  {
    display.setTextSize(1);
    display.print(F("Feels Like:"));
    display.print(F("\n\n"));
    //
    display.setTextSize(2);
    display.print(myObject["main"]["feels_like"]);
    display.print(F(" \xF8\x43"));
  }
    menu = WIND;
    break;
  case WIND:
  {
    display.setTextSize(1);
    display.print(F("Wind:"));
    display.print(F("\n\n"));
    //
    display.setTextSize(2);
    display.print(myObject["wind"]["speed"]);
    display.print(F(" m/s"));
    display.print(F("\n"));
    //
    display.setTextSize(1);
    display.print(F("\n"));
    //
    display.setTextSize(2);
    display.print(myObject["wind"]["deg"]);
    display.print(F(" \xF8"));
  }
    menu = CLOUDS;
    break;
  case CLOUDS:
  {
    display.setTextSize(1);
    display.print(F("Cloudiness:"));
    display.print(F("\n\n"));
    //
    display.setTextSize(2);
    display.print(myObject["clouds"]["all"]);
    display.print(F(" %"));
  }
    menu = WEATHER;
    break;
  }
  display.display();
  delay(TIME_VIEW);
}