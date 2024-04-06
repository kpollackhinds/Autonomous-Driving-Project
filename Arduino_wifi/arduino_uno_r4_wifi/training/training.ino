//**Notes for next time**//
// add print statements to check if velocities are being sent
// try with camera

//Code to be run on Arduino Uno R4 to record motor speeds

//ssid, password and ip_address defined in "secrets.h" files
#include "secrets.h"

#include <WiFiS3.h>
//PID line follower
#include <QTRSensors.h>

#define LEFT 0
#define RIGHT 1

//intervals at which to send speed data
#define INTERVAL 2 

// Line Sensor Properties
#define NUM_SENSORS             6  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
// #define EMITTER_PIN             QTR_NO_EMITTER_PIN  // emitter is controlled by digital pin 2
#define EMITTER_PIN             8  // emitter is controlled by digital pin 8

#define rightMaxSpeed 180 // max speed of the robot
#define leftMaxSpeed 180 // max speed of the robot
#define rightBaseSpeed 90 // this is the speed at which the motors should spin when the robot is perfectly on the line
#define leftBaseSpeed 90 // this is the speed at which the motors should spin when the robot is perfectly on the line

//will determine if we should start training
bool followLine = false;

QTRSensorsAnalog qtra((unsigned char[]) {A5, A4, A3, A2, A1, A0}, NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];
const int buttonPin = 12;

// PID Properties
const double KP = 0.14;
const double KD = 0.0;
double lastError = 0;
const int GOAL = 2400;

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

void stop();
void calibrateLineSensor();
void setMotor(int dir, int pwmVal, int enable, int in1, int in2);
void setMotors(int pwmVal_Left, int pwmVal_Right);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(10);

  // Initialize line sensor array
  calibrateLineSensor();

  pinMode(buttonPin, INPUT);

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
    client.print(v1Filt[RIGHT]);
    client.print(",");
    client.print((v1Filt[LEFT])*-1);
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
    Serial.println("stopping sending data");
  }

  if (followLine){
    // Get line position
    unsigned int position = qtra.readLine(sensorValues, QTR_EMITTERS_ON, 1);

    // Compute error from line
    int error = GOAL - position;

    // Compute motor adjustment
    int adjustment = KP*error + KD*(error - lastError);

    // Store error for next increment
    lastError = error;

    // Adjust motors 
    int rightMotorSpeed = rightBaseSpeed + adjustment;
    int leftMotorSpeed = leftBaseSpeed - adjustment;

    if (rightMotorSpeed > rightMaxSpeed ) rightMotorSpeed = rightMaxSpeed; // prevent the motor from going beyond max speed
    if (leftMotorSpeed > leftMaxSpeed ) leftMotorSpeed = leftMaxSpeed; // prevent the motor from going beyond max speed
    if (rightMotorSpeed < 0) rightMotorSpeed = 0; // keep the motor speed positive
    if (leftMotorSpeed < 0) leftMotorSpeed = 0; // keep the motor speed positive

    setMotors(leftMotorSpeed, rightMotorSpeed);
  }

  //manual button commands using default speed of 50
  if (followLine == false){
    if (strcmp(line, "stp") == 0){
      stop();
    }
  }
    
  //reset the line array to an array of zeros in the buffer size
  memset(line, 0, maxBufferSize);

  if (client.connected () == 0) {
    client.stop();
    WiFi.disconnect();
    WiFi.begin(ssid, password);

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

void setMotor(int pwmVal, int enable, int in1, int in2, int dir = 1) {
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

void setMotors(int pwmVal_Left, int pwmVal_Right) {
  digitalWrite(motorRight_InputOne, HIGH);
  digitalWrite(motorRight_InputTwo, LOW);
  analogWrite(motorRight_Enable, pwmVal_Right);
  digitalWrite(motorLeft_InputOne, LOW);
  digitalWrite(motorLeft_InputTwo, HIGH);
  analogWrite(motorLeft_Enable, pwmVal_Left);
}

void stop() {
  Serial.println("PAUSE");
  digitalWrite(motorLeft_InputOne, LOW); 
  digitalWrite(motorLeft_InputTwo, LOW); 
  digitalWrite(motorRight_InputOne, LOW); 
  digitalWrite(motorRight_InputTwo, LOW); 
}

void calibrateLineSensor() {
  delay(500);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);    // turn on Arduino's LED to indicate we are in calibration mode
  for (int i = 0; i < 3000; i++)  // make the calibration take about 10 seconds
  {
    qtra.calibrate();       // reads all sensors 10 times at 2.5 ms per six sensors (i.e. ~25 ms per call)
  }
  digitalWrite(13, LOW);     // turn off Arduino's LED to indicate we are through with calibration
  Serial.println("done");
}