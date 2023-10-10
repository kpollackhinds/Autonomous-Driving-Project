#include "secrets.h"


#include <WiFi.h>
#include <WiFiMulti.h>

//ssid, password and ip_address defined in "secrets.h" files

//Motor Pin Declarations
const int motorLeft_InputTwo = A3;
const int motorLeft_InputOne = A5;

const int motorRight_InputThree = A7;
const int motorRight_InputFour = A0;

String line;
//create wifi client object
WiFiClient client;
WiFiMulti WiFiMulti;

void forward(unsigned int speed = 100);
// void reverse();
// void turnright();
// void turnleft();
void stop();

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(A3, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(A7, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(A6, OUTPUT);

  digitalWrite(motorLeft_Enable, HIGH); 
  digitalWrite(motorRight_Enable, HIGH);

  // We start by connecting to a WiFi network
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);

  Serial.print("Connecting to ");
  Serial.println(ip_address);

  //connecting to socket 
  while (!client.connect(ip_address, REMOTE_PORT)) {
    Serial.println("Connection failed.");
    Serial.println("Waiting a moment before retrying...");
  }

  Serial.println("Connected");
  client.print("Hello\n");
  client.print("This is my IP.\n");
}

void loop() {
  if (client.available() > 0) {
  //  delay(20);
  //read back one line from the server
  line = client.readStringUntil('\n');
  // Serial.println(line);
  }

  if (line == "frwd"){
    Serial.println(line);
    forward();
  }
  else if (line == "stp"){
    Serial.println("stop");
    stop();
  }

  if (client.connected () == 0) {
  client.stop();
  WiFi.disconnect();
  }
}

//Sets one motor to rotate CW and the other to rotate CCW. 
void forward(unsigned int speed){  
  Serial.println("Forward.");
  digitalWrite(motorLeft_InputOne, HIGH);
  digitalWrite(motorLeft_InputTwo, LOW);
  digitalWrite(motorRight_InputThree  , HIGH);
  digitalWrite(motorRight_InputFour, LOW);
}

void reverse(unsigned int speed = 100) {
  Serial.println("Reverse");
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, HIGH); 
  digitalWrite(motorRight_InputThree, LOW); 
  digitalWrite(motorRight_InputFour, HIGH); 
  
}

void turnright(unsigned int speed = 100) {
  Serial.println("Turn Right");
  digitalWrite(motorLeft_InputOne, HIGH); 
  digitalWrite(motorLeft_InputTwo, LOW); 
  digitalWrite(motorRight_InputThree, LOW); 
  digitalWrite(motorRight_InputFour, HIGH); 
  
}

void turnleft(unsigned int speed) {
  Serial.println("Turn Left");
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, HIGH); 
  digitalWrite(motorRight_InputThree, HIGH); 
  digitalWrite(motorRight_InputFour, LOW); 
  
}

void stop() {
  Serial.println("PAUSE");
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, LOW); 
  digitalWrite(motorRight_InputThree, LOW); 
  digitalWrite(motorRight_InputFour, LOW); 
  
}