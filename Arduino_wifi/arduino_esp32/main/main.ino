#include "secrets.h"


#include <WiFi.h>
#include <WiFiMulti.h>

//ssid, password and ip_address defined in "secrets.h" files

//Motor Pin Declarations
const int motorLeft_InputTwo = A3;
const int motorLeft_InputOne = A5;

const int motorRight_InputThree = A7;
const int motorRight_InputFour = A0;

//max number of characters sent per message + null terminator
const byte maxBufferSize = 13; 
//initializing buffer to store characters 
char line[maxBufferSize];
//initializing vector to store commands parsed from lines recieved
int commands[3]; 
int i = 0;
//initializing token pointer for strtok method
char* token;
//create wifi client object
WiFiClient client;
WiFiMulti WiFiMulti;

int num_readings = 0;

void forward(unsigned int speed = 50);
void reverse(unsigned int speed = 50);
void turnright(unsigned int speed = 50);
void turnleft(unsigned int speed = 50);
void stop();
void go(unsigned int left_speed, unsigned int right_speed, int dir = 1);

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(A3, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(A7, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(A6, OUTPUT);

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
    //reads bytes in until new-line character is seen, stores them in the "line" buffer. Also returns the number of bytes read
    byte size = client.readBytesUntil('\n', line, maxBufferSize);

    line[size] = '\0';  //Null-terminator for the byte array
    // Serial.println(line);
  

    //manual button commands using default speed of 50
    if (strcmp(line, "frwd") == 0){
      forward();
    }
    else if (strcmp(line, "bck") == 0){
      reverse();
    }
    
    else if (strcmp(line, "lft") == 0){
      turnleft();
    }
    
    else if (strcmp(line, "rght") == 0){
      turnright();
    }
    else if (strcmp(line, "stp") == 0){
      stop();
    }

    else{
      i = 0;
      //splits line buffer on "," character, converts to int, and then stores in the command vector
      token = strtok(line, ",");

      while (token != NULL){
        commands[i] = atoi(token);
        i++;
        token = strtok(NULL, ",");
      }


      //sends commands to go function
      // if (commands[1] != 0 || commands[2] !=0){
        // Serial.println(commands[1]);
        // Serial.println(commands[2]);
        // Serial.println(commands[0]);
      go(commands[1], commands[2], commands[0]);
      // }
    }
  }
  //reset the line array to an array of zeros in the buffer size
  memset(line, 0, maxBufferSize);

  if (client.connected () == 0) {
  client.stop();
  WiFi.disconnect();
  }
}

void go(unsigned int left_speed, unsigned int right_speed, int dir){
  unsigned int mapped_left_speed = map(left_speed, 0, 100, 0, 255);
  unsigned int mapped_right_speed = map(right_speed, 0, 100, 0, 255);

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
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  digitalWrite(motorLeft_InputOne, HIGH);
  digitalWrite(motorLeft_InputTwo, LOW);
  digitalWrite(motorRight_InputThree  , HIGH);
  digitalWrite(motorRight_InputFour, LOW);
}

void reverse(unsigned int speed) {
  Serial.println("Reverse");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, HIGH); 
  digitalWrite(motorRight_InputThree, LOW); 
  digitalWrite(motorRight_InputFour, HIGH); 
  
}

void turnright(unsigned int speed) {
  Serial.println("Turn Right");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  digitalWrite(motorLeft_InputOne, HIGH); 
  digitalWrite(motorLeft_InputTwo, LOW); 
  digitalWrite(motorRight_InputThree, LOW); 
  digitalWrite(motorRight_InputFour, HIGH); 
  
}

void turnleft(unsigned int speed) {
  Serial.println("Turn Left");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
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