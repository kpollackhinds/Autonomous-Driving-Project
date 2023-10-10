#include <WiFiS3.h>
#include "secrets.h"
//ssid, password and ip_address defined in "secrets.h" files

//Motor Pin Declarations
const int motorLeft_InputTwo = 4;
const int motorLeft_InputOne = 7;
const int motorLeft_Enable = 6;

const int motorRight_Enable = 11;
const int motorRight_InputOne = 12;
const int motorRight_InputTwo = 10;

String line;
//create wifi client object
WiFiClient client;
void forward(unsigned int speed = 100);
void reverse();
void turnright();
void turnleft();
void pause();

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(motorLeft_Enable, HIGH); 
  digitalWrite(motorRight_Enable, HIGH);

  //connecting to wifi
  WiFi.begin(ssid, password);
  Serial.print("\nWaiting for WiFi... ");

  while (WiFi.status() != WL_CONNECTED) {
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
    pause();
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
  digitalWrite(motorRight_InputOne, HIGH);
  digitalWrite(motorRight_InputTwo, LOW);
}

void reverse(unsigned int speed = 100) {
  Serial.println("Reverse");
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, HIGH); 
  digitalWrite(motorRight_InputOne, LOW); 
  digitalWrite(motorRight_InputTwo, HIGH); 
  
}

void turnright(unsigned int speed = 100) {
  Serial.println("Turn Right");
  digitalWrite(motorLeft_InputOne, HIGH); 
  digitalWrite(motorLeft_InputTwo, LOW); 
  digitalWrite(motorRight_InputOne, LOW); 
  digitalWrite(motorRight_InputTwo, HIGH); 
  
}

void turnleft(unsigned int speed) {
  Serial.println("Turn Left");
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, HIGH); 
  digitalWrite(motorRight_InputOne, HIGH); 
  digitalWrite(motorRight_InputTwo, LOW); 
  
}

void pause() {
  Serial.println("PAUSE");
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, LOW); 
  digitalWrite(motorRight_InputOne, LOW); 
  digitalWrite(motorRight_InputTwo, LOW); 
  
}

