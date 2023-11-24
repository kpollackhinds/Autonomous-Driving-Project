//**Notes for next time**//
// add print statements to check if velocities are being sent
// try with camera

//Code to be run on Arduino Uno R4 to record motor speeds

//ssid, password and ip_address defined in "secrets.h" files
#include "secrets.h"

#include <WiFiS3.h>

#define LEFT 0
#define RIGHT 1

//intervals at which to send speed data
#define INTERVAL 2 

//will determine if we should start training
bool followLine = false;

//Motor Pin Declarations
const int motorLeft_Enable = 5;
const int motorLeft_InputOne = 6;
const int motorLeft_InputTwo = 7;

const int motorRight_Enable = 9;
const int motorRight_InputOne = 10;
const int motorRight_InputTwo = 11;

//Encoder Pin Declarations
const int leftENCA = 2;
const int leftENCB = 4;
const int rightENCA = 3;
const int rightENCB = 13;

//Arrays used to make access through function a little easier
const int enca[] = { leftENCA, rightENCA };
const int encb[] = { leftENCB, rightENCB };

long currTime = 0;
long prevTime = 0;

//values accessed/modified within Interrupt
volatile float velocity_i[2] = { 0.0, 0.0 };
volatile long prevT_i[2] = { 0, 0 };

//parameters for low pass filiter
float v1Filt[] = {0,0};
float v1Prev[] = {0,0};

//max number of characters sent per message + null terminator
const byte maxBufferSize = 13; 

//initializing buffer to store characters 
char line[maxBufferSize];

//initializing vector to store commands parsed from lines recieved
int commands[3];  

//initializing token pointer for strtok method
char* token;

//create wifi client object
WiFiClient client;

//function prototypes

void forward(unsigned int speed = 50);
void reverse(unsigned int speed = 50);
void turnright(unsigned int speed = 50);
void turnleft(unsigned int speed = 50);
void stop();
void go(unsigned int left_speed, unsigned int right_speed, int dir = 1);
void setMotor(int dir, int pwmVal, int enable, int in1, int in2);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(10);

  pinMode(motorLeft_Enable, OUTPUT);
  pinMode(motorLeft_InputTwo, OUTPUT);
  pinMode(motorLeft_InputOne, OUTPUT);

  pinMode(motorRight_Enable, OUTPUT);
  pinMode(motorRight_InputOne, OUTPUT);
  pinMode(motorRight_InputTwo, OUTPUT);

  pinMode(leftENCA, INPUT);
  pinMode(leftENCB, INPUT);
  pinMode(rightENCA, INPUT);
  pinMode(rightENCB, INPUT);

  //interrupt triggered during rising edge of pinA of each encoder
  attachInterrupt(digitalPinToInterrupt(leftENCA), readEncoder<LEFT>, RISING);
  attachInterrupt(digitalPinToInterrupt(rightENCA), readEncoder<RIGHT>, RISING);

  // We start by connecting to a WiFi network
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while(WiFi.status() != WL_CONNECTED) {
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

  float velocity2[2] = { 0, 0 };

  noInterrupts();
    velocity2[LEFT] = velocity_i[LEFT];
    velocity2[RIGHT] = velocity_i[RIGHT];
  interrupts();
  
  float v2[2] = { velocity2[LEFT] / 960 * 60, velocity2[RIGHT] / 960 * 60 };

  // Low-pass filter (25 Hz cutoff)
  v1Filt[LEFT] = 0.854*v1Filt[LEFT] + 0.0728*v2[LEFT] + 0.0728*v1Prev[LEFT];
  v1Prev[LEFT] = v2[LEFT];

  v1Filt[RIGHT] = 0.854*v1Filt[RIGHT] + 0.0728*v2[RIGHT] + 0.0728*v1Prev[RIGHT];
  v1Prev[RIGHT] = v2[RIGHT];

  currTime = micros();

  if ((currTime - prevTime)/1e6 >= INTERVAL && followLine){
    client.print("_");
    client.print(v1Filt[LEFT]);
    client.print(",");
    client.print(v1Filt[RIGHT]);
    client.print("\n");

    Serial.println("sent");
    prevTime = currTime;

  }

  if (client.available() > 0) {
    //read back one line from the server
    //reads bytes in until new-line character is seen, stores them in the "line" buffer. Also returns the number of bytes read
    byte size = client.readBytesUntil('\n', line, maxBufferSize);

    line[size] = '\0';  //Null-terminator for the byte array
    // Serial.println(line);
  }
    //check for starting training
  if (strcmp(line, "strt") == 0){
    Serial1.println("start");
    Serial.println("start");
    followLine = true;
  }

  //check for stopping training
  if (strcmp(line, "stpt") == 0){
    followLine = false;
    stop();
  }

  //manual button commands using default speed of 50
  if (followLine == false){
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
  }
    
  //reset the line array to an array of zeros in the buffer size
  memset(line, 0, maxBufferSize);

  if (client.connected () == 0) {
  client.stop();
  WiFi.disconnect();
  } 
  
  delay(1); //delay used to set a fixed sampling rate for low pass filter
}

//read encoder function checks speed and direction of motor. Gets triggered by interrupt defined in setup
template<int j>
void readEncoder() {
  int b = digitalRead(encb[j]);
  int increment = 0;
  if (b > 0) {
    increment = 1;
  } else {
    increment = -1;
  }

  long currT = micros();
  float deltaT = (float)(currT - prevT_i[j]) / 1000000;

  velocity_i[j] = increment / deltaT;

  prevT_i[j] = currT;
}

void setMotor(int dir, int pwmVal, int enable, int in1, int in2) {
  analogWrite(enable, pwmVal);
  if (dir == 1) {
    digitalWrite(in2, LOW);
    digitalWrite(in1, HIGH);
  } else if (dir == -1) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}

//function to move motor based on joystick commands
void go(unsigned int left_speed, unsigned int right_speed, int dir){
  unsigned int mapped_left_speed = map(left_speed, 0, 100, 0, 255);
  unsigned int mapped_right_speed = map(right_speed, 0, 100, 0, 255);
  Serial.println("GOING");
  //condition for going backwards
  if (dir == -1){
    analogWrite(motorLeft_InputOne, 0); 
    analogWrite(motorLeft_InputTwo, mapped_left_speed); 
    analogWrite(motorRight_InputOne, 0); 
    analogWrite(motorRight_InputTwo, mapped_right_speed);
  }
  //forwards motion (default)
  else{
    analogWrite(motorLeft_InputOne, mapped_left_speed);
    analogWrite(motorLeft_InputTwo, 0);
    analogWrite(motorRight_InputOne  , mapped_right_speed);
    analogWrite(motorRight_InputTwo, 0);
  }

}

void forward(unsigned int speed){  
  Serial.println("Forward");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_Enable, mapped_speed);
  analogWrite(motorRight_Enable, mapped_speed);

  digitalWrite(motorLeft_InputOne, HIGH);
  digitalWrite(motorLeft_InputTwo, 0);
  digitalWrite(motorRight_InputOne  , HIGH);
  digitalWrite(motorRight_InputTwo, 0);
}

void reverse(unsigned int speed) {
  Serial.println("Reverse");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_Enable, mapped_speed);
  analogWrite(motorRight_Enable, mapped_speed);

  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, HIGH); 
  digitalWrite(motorRight_InputOne, LOW); 
  digitalWrite(motorRight_InputTwo, HIGH); 
}

void turnright(unsigned int speed) {
  Serial.println("Turn Right");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_Enable, mapped_speed);
  analogWrite(motorRight_Enable, mapped_speed);

  digitalWrite(motorLeft_InputOne, HIGH); 
  digitalWrite(motorLeft_InputTwo, LOW); 
  digitalWrite(motorRight_InputOne, LOW); 
  digitalWrite(motorRight_InputTwo, HIGH);  
}

void turnleft(unsigned int speed) {
  Serial.println("Turn Left");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_Enable, mapped_speed);
  analogWrite(motorRight_Enable, mapped_speed);
  
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, HIGH); 
  digitalWrite(motorRight_InputOne, HIGH); 
  digitalWrite(motorRight_InputTwo, LOW); 
}

void stop() {
  Serial.println("PAUSE");
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, LOW); 
  digitalWrite(motorRight_InputOne, LOW); 
  digitalWrite(motorRight_InputTwo, LOW); 
}