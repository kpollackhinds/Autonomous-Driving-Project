//Two motor speed control PID

#define LEFT 0
#define RIGHT 1

class SimplePID {
public:
  SimplePID()
    : kp(1), kd(0), ki(0), umax(255), eprev(0.0), eintegral(0.0) {}  //constructor

  void setParams(float kpIn, float kdIn, float kiIn, float umaxIn, float eprevIn, float eintegralIn) {

    if (kpIn != -1.0) { kp = kpIn; }
    if (kdIn != -1.0) { kd = kdIn; }
    if (kiIn != -1.0) { ki = kiIn; }
    if (umaxIn != -1.0) { umax = umaxIn; }
    if (eprevIn != -1.0) { eprev = eprevIn; }
    if (eintegralIn != -1.0) { eintegral = eintegralIn; }
  }

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

long prevT[] = { 0, 0 };
long prevTime = 0;

//values accessed/modified within Interrupt
volatile float velocity_i[2] = { 0.0, 0.0 };
volatile long prevT_i[2] = { 0, 0 };

// float v1Filt = 0;
// float v1Prev = 0;
// float v2Filt = 0;
// float v2Prev = 0;

SimplePID pid_left;
SimplePID pid_right;

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

  //interrupt triggered during rising edge of pinA of encoder
  attachInterrupt(digitalPinToInterrupt(leftENCA), readEncoder<LEFT>, RISING);
  attachInterrupt(digitalPinToInterrupt(rightENCA), readEncoder<RIGHT>, RISING);

  pid_left.setParams(5, 0, 10, -1.0, -1.0, -1.0);
  pid_right.setParams(5, 0, 10, -1.0, -1.0, -1.0);
}

void loop() {
  float velocity2[2] = { 0, 0 };


  noInterrupts();
    velocity2[LEFT] = velocity_i[LEFT];
    velocity2[RIGHT] = velocity_i[RIGHT];
  interrupts();

  float v2[2] = { velocity2[LEFT] / 960 * 60, velocity2[RIGHT] / 960 * 60 };

  //filtering stuff//
  long currTime = micros();
  float deltaTime = (float(currTime - prevTime)) / (1.0e6);
  //set target for left and right
  float vt_l = 50 * (sin(currTime / 1e6) > 0);
  float vt_r = 50 * (sin(currTime / 1e6) > 0);

  int pwr_l, pwr_r;
  int dir_l, dir_r;


  pid_left.evalu(v2[LEFT], vt_l, deltaTime, pwr_l, dir_l);
  pid_right.evalu(v2[RIGHT], vt_r, deltaTime, pwr_r, dir_r);
  prevTime = currTime;

  //driving left motor
  setMotor(dir_l, pwr_l, motorLeft_Enable, motorLeft_InputOne, motorLeft_InputTwo);
  setMotor(dir_r, pwr_r, motorRight_Enable, motorRight_InputOne, motorRight_InputTwo);
  Serial.print(pwr_l);
  Serial.print(",");
  Serial.print(vt_r);
  Serial.print(",");
  Serial.print(v2[RIGHT]);
  Serial.print(",");
  Serial.println(v2[LEFT]);

  delay(1);
}


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
