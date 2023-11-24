//PID line follower intended for Arduino Nano ESP32
#include <QTRSensors.h>

// Line Sensor Properties
#define NUM_SENSORS             8  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             QTR_NO_EMITTER_PIN  // emitter is controlled by digital pin 2

#define rightMaxSpeed 255 // max speed of the robot
#define leftMaxSpeed 255 // max speed of the robot
#define rightBaseSpeed 155 // this is the speed at which the motors should spin when the robot is perfectly on the line
#define leftBaseSpeed 155 // this is the speed at which the motors should spin when the robot is perfectly on the line

QTRSensorsAnalog qtra((unsigned char[]) {A0, A1, A2, A3, A4, A5, A6, A7}, NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];

//Motor Pin Declarations
const int motorLeft_Enable = 4;
const int motorLeft_InputOne = 3;
const int motorLeft_InputTwo = 2;

const int motorRight_Enable = 7;
const int motorRight_InputOne = 8;
const int motorRight_InputTwo = 9;

// PID Properties
const double KP = 0.02;
const double KD = 0.0;
double lastError = 0;
const int GOAL = 3500;
const unsigned char MAX_SPEED = 50;

bool start = false;


void setup() {
  Serial1.begin(115200);

  // Initialize line sensor array
  calibrateLineSensor();

  pinMode(motorRight_InputOne, OUTPUT);
  pinMode(motorRight_InputTwo, OUTPUT);
  pinMode(motorRight_Enable, OUTPUT);
  pinMode(motorLeft_InputOne, OUTPUT);
  pinMode(motorLeft_InputTwo, OUTPUT);
  pinMode(motorLeft_Enable, OUTPUT);
}

void loop() {

  if (Serial1.available()){
    if (Serial1.readString() == "start"){
      start = true;
    }
    else{
      start = false;
    }
  }

  if (start){
    // Get line position
    unsigned int position = qtra.readLine(sensorValues);

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

    
    digitalWrite(motorRight_InputOne, HIGH);
    digitalWrite(motorRight_InputTwo, LOW);
    analogWrite(motorRight_Enable, rightMotorSpeed);
    digitalWrite(motorLeft_InputOne, HIGH);
    digitalWrite(motorLeft_InputTwo, LOW);
    analogWrite(motorLeft_Enable, leftMotorSpeed);
  
  }
  

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
}