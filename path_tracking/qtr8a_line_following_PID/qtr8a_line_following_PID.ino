//PID line follower 
#include <QTRSensors.h>

// Line Sensor Properties
#define NUM_SENSORS             6  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
// #define EMITTER_PIN             QTR_NO_EMITTER_PIN  // emitter is controlled by digital pin 2
#define EMITTER_PIN             8  // emitter is controlled by digital pin 2

// #define rightMaxSpeed 255 // max speed of the robot
// #define leftMaxSpeed 255 // max speed of the robot
// #define rightBaseSpeed 155 // this is the speed at which the motors should spin when the robot is perfectly on the line
// #define leftBaseSpeed 155 // this is the speed at which the motors should spin when the robot is perfectly on the line

#define rightMaxSpeed 200 // max speed of the robot
#define leftMaxSpeed 200 // max speed of the robot
#define rightBaseSpeed 160 // this is the speed at which the motors should spin when the robot is perfectly on the line
#define leftBaseSpeed 160 // this is the speed at which the motors should spin when the robot is perfectly on the line

QTRSensorsAnalog qtra((unsigned char[]) {A5, A4, A3, A2, A1, A0}, NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];

//Motor Pin Declarations
const int motorLeft_Enable = 5;
const int motorLeft_InputOne = 6;
const int motorLeft_InputTwo = 7;

const int motorRight_Enable = 9;
const int motorRight_InputOne = 10;
const int motorRight_InputTwo = 11;

const int buttonPin = 12;

// PID Properties
const double KP = 0.08;
const double KD = 0.0;
double lastError = 0;
// const int GOAL = 3500;
const int GOAL = 2400;

const unsigned char MAX_SPEED = 50;

bool start = false;

unsigned long lastDebounceTime = 0;
int buttonState = LOW;
const long debounceDelay = 200;


void setup() {
  Serial.begin(9600);
  delay(100);

  // Initialize line sensor array
  calibrateLineSensor();

  pinMode(motorRight_InputOne, OUTPUT);
  pinMode(motorRight_InputTwo, OUTPUT);
  pinMode(motorRight_Enable, OUTPUT);
  pinMode(motorLeft_InputOne, OUTPUT);
  pinMode(motorLeft_InputTwo, OUTPUT);
  pinMode(motorLeft_Enable, OUTPUT);

  pinMode(buttonPin, INPUT);
}

void loop() {
  //start when i push button. If its started and the button is pushed again, stop
  // Serial.println(digitalRead(buttonPin));

  int reading = digitalRead(buttonPin);
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the start if the new button state is HIGH
      if (buttonState == HIGH) {
        start = !start;

        if (start) {
          Serial.println("starting");
        } else {
          Serial.println("stopping");
          analogWrite(motorLeft_Enable, 0);
          analogWrite(motorRight_Enable, 0);
        }
      }
    }
  }

  if (start){
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


    Serial.print(rightMotorSpeed);
    Serial.print(",");
    Serial.println(leftMotorSpeed);
    
    digitalWrite(motorRight_InputOne, HIGH);
    digitalWrite(motorRight_InputTwo, LOW);
    analogWrite(motorRight_Enable, rightMotorSpeed);
    digitalWrite(motorLeft_InputOne, LOW);
    digitalWrite(motorLeft_InputTwo, HIGH);
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
  Serial.println("done");
}