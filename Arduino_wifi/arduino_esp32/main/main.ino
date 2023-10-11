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

void forward(unsigned int speed = 50);
void reverse(unsigned int speed = 50);
void turnright(unsigned int speed = 50);
void turnleft(unsigned int speed = 50);
void stop(unsigned int speed = 50);

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
  //read back one line from the server
  line = client.readStringUntil('\n');
  // Serial.println(line);
  }

  //manual button commands using default speed of 50
  if (line == "frwd"){
    forward();
  }
  else if (line == "bck"){
    reverse();
  }
  else if (line == "lft"){
    turnleft();
  }
  else if (line == "rght"){
    turnright();
  }
  else if (line == "stp"){
    stop();
  }

  else{
    
  }
  

  if (client.connected () == 0) {
  client.stop();
  WiFi.disconnect();
  }
}

void go(unsigned int left_speed, unsigned int right_speed, unsigned int dir = 1){
  Serial.println("brr");
  mapped_left_speed = map(left_speed, 0, 100, 0, 255);
  mapped_right_speed = map(right_speed, 0, 100, 0, 255);

  //condition for going backwards
  if (dir == -1){
    digitalWrite(motorLeft_InputOne, LOW); 
    analogWrite(motorLeft_InputTwo, mapped_left_speed); 
    digitalWrite(motorRight_InputThree, LOW); 
    analogWrite(motorRight_InputFour, mapped_right_speed);
  }
  //forwards motion (default)
  else{
    digitalWrite(motorLeft_InputOne, mapped_left_speed);
    digitalWrite(motorLeft_InputTwo, LOW);
    digitalWrite(motorRight_InputThree  , mapped_right_speed);
    digitalWrite(motorRight_InputFour, LOW);
  }

}

void forward(unsigned int speed){  
  Serial.println("Forward");
  mapped_speed = map(speed, 0, 100, 0, 255);
  digitalWrite(motorLeft_InputOne, HIGH);
  digitalWrite(motorLeft_InputTwo, LOW);
  digitalWrite(motorRight_InputThree  , HIGH);
  digitalWrite(motorRight_InputFour, LOW);
}

void reverse(unsigned int speed = 100) {
  Serial.println("Reverse");
  mapped_speed = map(speed, 0, 100, 0, 255);
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, HIGH); 
  digitalWrite(motorRight_InputThree, LOW); 
  digitalWrite(motorRight_InputFour, HIGH); 
  
}

void turnright(unsigned int speed = 100) {
  Serial.println("Turn Right");
  mapped_speed = map(speed, 0, 100, 0, 255);
  digitalWrite(motorLeft_InputOne, HIGH); 
  digitalWrite(motorLeft_InputTwo, LOW); 
  digitalWrite(motorRight_InputThree, LOW); 
  digitalWrite(motorRight_InputFour, HIGH); 
  
}

void turnleft(unsigned int speed) {
  Serial.println("Turn Left");
  mapped_speed = map(speed, 0, 100, 0, 255);
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