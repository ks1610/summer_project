/*
FS  //đi thẳng
RS  //rẽ phải
LS  //rẽ trái  
BS  //đi lùi

FL //thẳng chéo trái
FR //thẳng chéo phải
BL //Lùi chéo trái
BR //Lùi chéo phải

RL //Quay trái 
RR //Quay phải

EM //Dừng khẩn cấp
*/

#include<SoftwareSerial.h>

/* Create object named bt of the class SoftwareSerial */ 
SoftwareSerial bt(6,7); /* (Tx,Rx) */	
String inputString = "";  // To store incoming data

void setup() {
  bt.begin(9600);	/* Define baud rate for software serial communication */
  Serial.begin(9600);	/* Define baud rate for serial communication */
}

void loop() {
  while (bt.available()) {
    char c = bt.read();
    inputString += c;        // Build the message
    
    // Keep buffer length at max 2
    if (inputString.length() > 2) {
      inputString = inputString.substring(1);  // Keep last 2 chars
    }

    Serial.print(c);         // Optional: echo to serial monitor
    

    // If we received a complete command
    if (inputString == "FS") {
      Serial.println(" => Go Forward");
      inputString = ""; // Clear after processing
    } 
    // If we received a complete command
    else if (inputString == "RS") {
      Serial.println(" => Go Right");
      inputString = ""; // Clear after processing
    } 
    // If we received a complete command
    else if (inputString == "LS") {
      Serial.println(" => Go Left");
      inputString = ""; // Clear after processing
    } 
    // If we received a complete command
    else if (inputString == "BS") {
      Serial.println(" => Go Back");
      inputString = ""; // Clear after processing
    } 

    // If we received a complete command
    if (inputString == "FL") {
      Serial.println(" => Cross Forward Left");
      inputString = ""; // Clear after processing
    } 
    // If we received a complete command
    else if (inputString == "FR") {
      Serial.println(" => Cross Forward Right");
      inputString = ""; // Clear after processing
    } 
    // If we received a complete command
    else if (inputString == "BL") {
      Serial.println(" => Cross Backward Left");
      inputString = ""; // Clear after processing
    } 
    // If we received a complete command
    else if (inputString == "BR") {
      Serial.println(" => Cross Backward Right");
      inputString = ""; // Clear after processing
    } 
    // If we received a complete command
    else if (inputString == "RL") {
      Serial.println(" => Turn Around Left");
      inputString = ""; // Clear after processing
    } 
    // If we received a complete command
    else if (inputString == "RR") {
      Serial.println(" => Turn Around  Right");
      inputString = ""; // Clear after processing
    } 
    // If we received a complete command
    else if (inputString == "EM") {
      Serial.println(" => Emercy -> Stop");
      inputString = ""; // Clear after processing
    } 
    else if(inputString.length() >= 2) {
      // If not matched and length is too long, reset
      inputString = "";
    }
  }
}
