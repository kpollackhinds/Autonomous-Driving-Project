#include "secrets.h"

#include <WiFi.h>
#include <WiFiMulti.h>
#include <util/atomic.h>

//ssid, password and ip_address defined in "secrets.h" files

//Motor Pin Declarations
const int motorLeft_InputTwo = A3;
const int motorLeft_InputOne = A5;
const int motorRight_InputThree = A7;
const int motorRight_InputFour = A0;

//Encoder Pin Declarations
// const int leftENCA =
// const int leftENCB =
// const int rightENCA =
// const int rightENCB =

//Arrays used to make access through function a little easier
const int enca[] = {leftENCA, rightENCA};
const int encb[] = {leftENCB, rightENCB};

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

int pos = 0;
long prevT = 0;
int posPrev = 0;
volatile int pos_i = 0;

//interrupt triggered during rising edge of pinA of encoder
attachInterrupt(digitalPinToInterrupt(leftENCA), readEncoder<0>, RISING);
attachInterrupt(digitalPintoInterrupt(rightENCA), readEncoder<1>, RISING);

ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
  pos = pos_i;
}

void forward(unsigned int speed = 50);
void reverse(unsigned int speed = 50);
void turnright(unsigned int speed = 50);
void turnleft(unsigned int speed = 50);
void stop();
void go(unsigned int left_speed, unsigned int right_speed, int dir = 1);

class SimplePID;
//making an array with 2 instances of PID controller object (2 motors)
SimplePID() pid[2];

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(A3, OUTPUT);
  pinMode(A7, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A0, OUTPUT);

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

      Serial.print(commands[0]);
      Serial.print(" ");
      Serial.print(commands[1]);
      Serial.print(" ");
      Serial.println(commands[2]);      //sends commands to go function
      if (commands[1] == 0 && commands[2] == 0){
        stop();
      }
      else{
        // Serial.println(commands[1]);
        // Serial.println(commands[2]);
        // Serial.println(commands[0]);
        go(commands[1], commands[2], commands[0]);
      }
    }
  }
  //reset the line array to an array of zeros in the buffer size
  memset(line, 0, maxBufferSize);

  if (client.connected () == 0) {
  client.stop();
  WiFi.disconnect();
  }
}

//defining PID class
class SimplePID(){
  public:
    SimplePID() : kp(1), kd(0), ki(0), umax(255), eprev(0.0), eintegral(0.0){} //constructor

  private:
     float kp, ki, kd, umax; //parameters
     float eprev, eintegral; //previous and integral errors
  
  //setter function
  void setParams(float kpIn, float kdIn, float kiIn, float umaxIn, float eprevIn, float eintegralIn){
    kp = kpIn;
    kd = kdIn;
    ki = kiIn;
    umax = umaxIn;
    eprev = eprevIn;
    eintegral = eintegralIn;
  }

  void evalu(int value, int target, float deltaT, int &pwr, int &dir){
    //current error
    int e = target - value;

    //integral error
    float eintegral = eintegral + e*deltaT;

    //derivative error
    float dedt = (e - eprev)/deltaT;
    
    //output signal
    float u = kp*e + ki*eintegral + kd*dedt;

    //motor power
    pwr = (int)fabs(u);
    if(pwr > umax){
      pwr = umax;
    }

    dir = 1;
    if(u < 0){
      dir = -1;
    }

    //storing previous error
    eprev = e;
  }
}

template <int j>
void readEncoder(){
  if (j == 0){
    //leftencoder stuff
  }
  else if (j ==1){
    //rightencoder stuff
  }
}

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2){

}
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
  analogWrite(motorLeft_InputOne, 255);
  analogWrite(motorLeft_InputTwo, 0);
  analogWrite(motorRight_InputThree  , 255);
  analogWrite(motorRight_InputFour, 0);
}

void reverse(unsigned int speed) {
  Serial.println("Reverse");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_InputOne, 0); 
  analogWrite(motorLeft_InputTwo, 255); 
  analogWrite(motorRight_InputThree, 0); 
  analogWrite(motorRight_InputFour, 255); 
  
}

void turnright(unsigned int speed) {
  Serial.println("Turn Right");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_InputOne, 255); 
  analogWrite(motorLeft_InputTwo, 0); 
  analogWrite(motorRight_InputThree, 0); 
  analogWrite(motorRight_InputFour, 255); 
  
}

void turnleft(unsigned int speed) {
  Serial.println("Turn Left");
  unsigned int mapped_speed = map(speed, 0, 100, 0, 255);
  analogWrite(motorLeft_InputOne, 0); 
  analogWrite(motorLeft_InputTwo, 255); 
  analogWrite(motorRight_InputThree, 255); 
  analogWrite(motorRight_InputFour, 0); 
  
}

void stop() {
  Serial.println("PAUSE");
  analogWrite(motorLeft_InputOne, 0); 
  analogWrite(motorLeft_InputTwo, 0); 
  analogWrite(motorRight_InputThree, 0); 
  analogWrite(motorRight_InputFour, 0); 
  Serial.println("done");
}