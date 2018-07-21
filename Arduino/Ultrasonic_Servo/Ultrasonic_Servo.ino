#include <Servo.h>
#include <NewPing.h>

// define pins and other constants
const int trigPin = A0;
const int echoPin = A1;
const int servo1Pin = 9;
const int max_distance = 300;
const int E1 = 3;
const int E2 = 11;
const int M1 = 12;
const int M2 = 13;


// variables to use later on
int dist;

// create instances of servo and ping class
Servo Servo1;
NewPing sonar(trigPin, echoPin, max_distance);

void setup()
{
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  pinMode(M1, OUTPUT);    // Sets Motor1, pin 12, as Output
  pinMode(M2, OUTPUT);    // Sets Motor2, pin 13, as Output
  Serial.begin(9600); // Starts the serial communication
  Servo1.attach(servo1Pin);   // connect servo to pin 0
}

void forward(dir, PWM){
  /*Moving the car forward*/
  digitalWrite(M1,HIGH);   
  digitalWrite(M2, HIGH);       
  analogWrite(E1, PWM);   //PWM Speed Control
  analogWrite(E2, PWM);   //PWM Speed Control
  delay(50);
}

void head(){
  /* Controls the servo, 90 is center, angle from 0~180, rotates really fast.*/
  Servo1.write(90);
  delay(3000);  //wait 3 seconds
  Servo1.write(0);
  delay(500);  //wait 0.5 seconds
  Servo1.write(90);
  delay(500);  //wait 0.5 seconds
  Servo1.write(180);
  delay(500);  //wait 0.5 seconds 
}

void lookAround(){
  /* Moving head around for a small angle, looking for target
   * if anything is within range of 30cm, follw on that direction
   * return a direction, 'LEFT', 'RIGHT', 'FORWARD', 'BACKWARD', 'STOP'
   */
  const char dir = Null
  Servo1.write(90);
  delay(50); 
  dist0 = sonar.ping_cm();   
  Servo1.write(80);
  delay(50);  
  dist1 = sonar.ping_cm();  
  Servo1.write(100);
  delay(20);  
  dist2 = sonar.ping_cm(); 
  
  return dir
}

void follow(){
  /* This function allows the car to follow a traget, 
   *  detects the distance from the traget using ultrasonic sensor,
   *  stops when about 15cm from the sensor,
   */
  follow = 0   //False by default
  while sonar.ping_cm() >= 15:
    forward();
    follow = 1   //True, following target target
  const char dir = lookAround()
  //PWM is decided based on distance it is from the target
  //forward(dir, PWM)
}

void loop()
{
  //Obtain readings from ultrasonic sensot
  Serial.print("Ping: ");
  dist = sonar.ping_cm();  
  Serial.print(dist);
  Serial.println("cm");
  delay(100);
  head();
  forward();

}


