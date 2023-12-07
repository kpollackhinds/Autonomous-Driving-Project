//L293D Motor Test
const int enable1 = 5;
//Motor A
const int motorPin1  = 6;  // Pin 14 of L293
const int motorPin2  = 7;  // Pin 10 of L293
//Motor B
const int enable2 = 9;
const int motorPin3  = 10; // Pin  7 of L293                  
const int motorPin4  = 11;  // Pin  2 of L293

//This will run only one time.
void setup(){
    Serial.begin(9600);
    //Set pins as outputs
    pinMode(enable1, OUTPUT);
    pinMode(enable2, OUTPUT);
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);
    
    //Motor Control - Motor A: motorPin1,motorpin2 & Motor B: motorpin3,motorpin4

    //This code  will turn Motor A clockwise for 2 sec.
    analogWrite(enable1, 255);
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(2000); 
    //This code will turn Motor A counter-clockwise for 2 sec.
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(1000);
    
    //This code will turn Motor B clockwise for 2 sec.
    Serial.println("wfat");
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(1000); 
    //This code will turn Motor B counter-clockwise for 2 sec.
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
    delay(1000);    
    
    //And this code will stop motors
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);

    
  
}


void loop(){
  //   digitalWrite(motorPin1, HIGH);
  //   digitalWrite(motorPin2, LOW);
  //   digitalWrite(motorPin3, LOW);
  //   digitalWrite(motorPin4, LOW);
  //   delay(2000); 
  //   //This code will turn Motor A counter-clockwise for 2 sec.
  //   digitalWrite(motorPin1, LOW);
  //   digitalWrite(motorPin2, HIGH);
  //   digitalWrite(motorPin3, LOW);
  //   digitalWrite(motorPin4, LOW);
  //   delay(2000);
  // //This code will turn Motor B clockwise for 2 sec.
  //   Serial.println("1");
  //   digitalWrite(motorPin1, LOW);
  //   digitalWrite(motorPin2, LOW);
  //   digitalWrite(motorPin3, HIGH);
  //   digitalWrite(motorPin4, LOW);
  //   delay(2000); 
  //   //This code will turn Motor B counter-clockwise for 2 sec.
  //   Serial.println("2");
  //   digitalWrite(motorPin1, LOW);
  //   digitalWrite(motorPin2, LOW);
  //   digitalWrite(motorPin3, LOW);
  //   digitalWrite(motorPin4, HIGH);
  //   delay(2000);    
  //   Serial.println("3");
  //   //And this code will stop motors
  //   digitalWrite(motorPin1, LOW);
  //   digitalWrite(motorPin2, LOW);
  //   digitalWrite(motorPin3, LOW);
  //   digitalWrite(motorPin4, LOW);
  //   delay(1000);

  //This code  will turn Motor A clockwise for 2 sec.
    analogWrite(enable1, 123);
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(2000); 
    //This code will turn Motor A counter-clockwise for 2 sec.
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(1000);
    
    //This code will turn Motor B clockwise for 2 sec.
    analogWrite(enable2, 123);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(1000); 
    //This code will turn Motor B counter-clockwise for 2 sec.
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
    delay(1000);    
    
    //And this code will stop motors
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);

    delay(2000);

    analogWrite(enable1, 255);
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(2000); 
    //This code will turn Motor A counter-clockwise for 2 sec.
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(1000);
    
    //This code will turn Motor B clockwise for 2 sec.
    analogWrite(enable2, 255);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(1000); 
    //This code will turn Motor B counter-clockwise for 2 sec.
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
    delay(1000);    
    
    //And this code will stop motors
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(500);

}