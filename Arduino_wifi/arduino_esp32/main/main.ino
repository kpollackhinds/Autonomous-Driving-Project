#include "secrets.h"

#include <WiFi.h>
#include <WiFiMulti.h>

//ssid, password and ip_address defined in "secrets.h" files

#define LEFT 0
#define RIGHT 1

//Class for PID control
class SimplePID {
public:
  SimplePID()
    : kp(1), kd(0), ki(0), umax(255), eprev(0.0), eintegral(0.0) {}  //constructor


  //lazy way to allow for the reassignment of parameters
  void setParams(float kpIn, float kdIn, float kiIn, float umaxIn, float eprevIn, float eintegralIn) {
    if (kpIn != -1.0) { kp = kpIn; }
    if (kdIn != -1.0) { kd = kdIn; }
    if (kiIn != -1.0) { ki = kiIn; }
    if (umaxIn != -1.0) { umax = umaxIn; }
    if (eprevIn != -1.0) { eprev = eprevIn; }
    if (eintegralIn != -1.0) { eintegral = eintegralIn; }
  }

  //function to determine command to motor based on deviation between expected value and actual measured
  void evalu(int value, int target, float deltaT, int &pwr, int &dir) {
    //current error
    int e = target - value;

    //integral error
    eintegral = eintegral + e * deltaT;

    //derivative error
    float dedt = (e - eprev) / deltaT;

    //output signal
    float u = kp * e + ki * eintegral + kd * dedt;
    // float u = kp*e + ki*eintegral;


    //motor power
    pwr = (int)fabs(u);
    if (pwr > umax) {
      pwr = umax;
    }

    dir = 1;
    if (u < 0) {
      dir = -1;
    }

    //storing previous error
    eprev = e;
  }

private:
  float kp, ki, kd, umax;  //parameters
  float eprev, eintegral;  //previous and integral errors
};

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

//values used within eval function of PID objects
long prevT[] = { 0, 0 };
long prevTime = 0;

//values accessed/modified within Interrupt
volatile float velocity_i[2] = { 0.0, 0.0 };
volatile long prevT_i[2] = { 0, 0 };

//parameters for low pass filiter
float v1Filt[] = {0,0};
float v1Prev[] = {0,0};

//creating instance of PID for left and right motors
// SimplePID pid_left;
// SimplePID pid_right;

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
WiFiMulti WiFiMulti;

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

  pid_left.setParams(5, 0, 10, -1.0, -1.0, -1.0);
  pid_right.setParams(5, 0, 10, -1.0, -1.0, -1.0);

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
  float velocity2[2] = { 0, 0 };

  noInterrupts();
    velocity2[LEFT] = velocity_i[LEFT];
    velocity2[RIGHT] = velocity_i[RIGHT];
  interrupts();
  
  float v2[2] = { velocity2[LEFT] / 960 * 60, velocity2[RIGHT] / 960 * 60 };

  // Low-pass filter (25 Hz cutoff)
  v1Filt[LEFT] = 0.854*v1Filt[LEFT] + 0.0728*v2 + 0.0728*v1Prev[LEFT];
  v1Prev[LEFT] = v2[LEFT];

  v1Filt[RIGHT] = 0.854*v1Filt[RIGHT] + 0.0728*v2 + 0.0728*v1Prev[RIGHT];
  v1Prev[RIGHT] = v2[RIGHT];

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

    // // Below is code to use for parsing commands obtained from using the joystick option.
    // else{
    //   i = 0;
    //   //splits line buffer on "," character, converts to int, and then stores in the command vector
    //   token = strtok(line, ",");

    //   while (token != NULL){
    //     commands[i] = atoi(token);
    //     i++;
    //     token = strtok(NULL, ",");
    //   }

    //   Serial.print(commands[0]);
    //   Serial.print(" ");
    //   Serial.print(commands[1]);
    //   Serial.print(" ");
    //   Serial.println(commands[2]);      //sends commands to go function
    //   if (commands[1] == 0 && commands[2] == 0){
    //     stop();
    //   }
    //   else{
    //     // Serial.println(commands[1]);
    //     // Serial.println(commands[2]);
    //     // Serial.println(commands[0]);
    //     go(commands[1], commands[2], commands[0]);
    //   }
    // }
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
    analogWrite(motorRight_InputThree, 0); 
    analogWrite(motorRight_InputFour, mapped_right_speed);
  }
  //forwards motion (default)
  else{
    analogWrite(motorLeft_InputOne, mapped_left_speed);
    analogWrite(motorLeft_InputTwo, 0);
    analogWrite(motorRight_InputThree  , mapped_right_speed);
    analogWrite(motorRight_InputFour, 0);
  }

}

void forward(unsigned int speed){  
  Serial.println("Forward");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_Enable, mapped_speed);
  analogWrite(motorRight_Enable, mapped_speed);

  digitalWrite(motorLeft_InputOne, HIGH);
  digitalWrite(motorLeft_InputTwo, 0);
  digitalWrite(motorRight_InputThree  , HIGH);
  digitalWrite(motorRight_InputFour, 0);
}

void reverse(unsigned int speed) {
  Serial.println("Reverse");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_Enable, mapped_speed);
  analogWrite(motorRight_Enable, mapped_speed);

  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, HIGH); 
  digitalWrite(motorRight_InputThree, LOW); 
  digitalWrite(motorRight_InputFour, HIGH); 
  
}

void turnright(unsigned int speed) {
  Serial.println("Turn Right");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_Enable, mapped_speed);
  analogWrite(motorRight_Enable, mapped_speed);

  digitalWrite(motorLeft_InputOne, HIGH); 
  digitalWrite(motorLeft_InputTwo, LOW); 
  digitalWrite(motorRight_InputThree, LOW); 
  digitalWrite(motorRight_InputFour, HIGH); 
  
}

void turnleft(unsigned int speed) {
  Serial.println("Turn Left");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_Enable, mapped_speed);
  analogWrite(motorRight_Enable, mapped_speed);
  
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
  Serial.println("done");
}