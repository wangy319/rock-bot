/*

*/
#include <SD.h>
#include <TMRpcm.h>
#define SD_Chipselect 11

#include <LedControl.h>
#include <Servo.h>
#include <NewPing.h>
#include "binary.h"


int E1 = 3;
int E2 = 11;
int M1 = 12;  //two motors
int M2 = 13;
int vibration = 4; //vibration motor

int trigPin = A0;   //ultrasonic sensor
int echoPin = A1;   //
int tempSen = A3;   //temperature sensor
int ardConnect_1 = A4;   //pins for serial communication to nano
int ardConnect_2 = A5;

// define pins and other constants
//const int servo1Pin = 9;
const int max_distance = 300;
char incomingByte;
int DIN = 0;   //dot matrix display 
int CLK = 1;
int CS = 2;
int maxInUse = 1;


// variables to use later on
int dist;
struct Coordinate {float x; float y;};

// create instances of servo and ping class
NewPing sonar(trigPin, echoPin, max_distance);




LedControl lc=LedControl(DIN,CLK,CS,1);


TMRpcm tmrpcm;
void setup() {
  tmrpcm.speakerPin = 9;
  tmrpcm.volume(1);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(vibration,OUTPUT);
  pinMode(M1,OUTPUT);
  pinMode(M2,OUTPUT);
  pinMode(4, OUTPUT); 
  
  
  lc.shutdown(0,false);       //The MAX72XX is in power-saving mode on startup
  lc.setIntensity(0,8);      // Set the brightness to maximum value
  lc.clearDisplay(0);
}

/* Direction control functions */
float get_angle(struct Coordinate target){
  float ang;   //relative angle between the car heading direction and target direction
               //Netative: on the left of heading direction; Positive: on the right of heading direction
  ang = atan(target.x/target.y)/3.1415926*180;
  return ang;        
}

void dir_update(){
  /* This function performs turning angle calculation, and make a turn. */
  Serial.println("Updating direction! \n");
  
  struct Coordinate new_target = get_target_coor();
  float target_dist = get_distance(new_target);
  float ang = get_angle(new_target);

//  Serial.println("Angle is ");
//  Serial.println(ang);
  
  // dir = 'RIGHT turn':1, 'LEFT turn':-1
  if(ang >= 5.0){
    while(ang >= 5.0){
      new_target = get_target_coor();
      target_dist = get_distance(new_target);
      ang = get_angle(new_target);
      turn(-1);
    }
  }else if(ang <= -5.0){
    while(ang >= 5.0){
      new_target = get_target_coor();
      target_dist = get_distance(new_target);
      ang = get_angle(new_target);
      turn(-1);
    }
  }else{
    forward(255);
    forward(255);
    forward(255);
  }
}

void forward(int PWM){
  /*Moving the car straightly forward*/
  Serial.print("Moving forward...\n ");
  digitalWrite(M1,HIGH);   
  digitalWrite(M2, HIGH);       
  analogWrite(E1, PWM);   //PWM Speed Control
  analogWrite(E2, PWM);   //PWM Speed Control
  delay(500);
}

void Stop(){
  /*Stop the car*/
  Serial.print("Stop...\n ");
  digitalWrite(M1, LOW);   
  digitalWrite(M2, LOW);       
  analogWrite(E1, 0);   //PWM Speed Control
  analogWrite(E2, 0);   //PWM Speed Control
  delay(500);
}

void turn(signed int dir){
  /*Input: dir = 'RIGHT turn':1, 'LEFT turn':-1
   * M1: right wheel
   * M2: left wheel
  */
  if(dir == 1){   //right turn
    Serial.print("Turning right...\n ");
    digitalWrite(M1, HIGH);   
    digitalWrite(M2, LOW);       
    analogWrite(E1, 255);   //PWM Speed Control
    analogWrite(E2, 0);   //PWM Speed Control
    delay(500);
  }else{        //left turn
    Serial.print("Turning left... \n");
    digitalWrite(M1, LOW);   
    digitalWrite(M2, HIGH);       
    analogWrite(E1, 0);   //PWM Speed Control
    analogWrite(E2, 255);   //PWM Speed Control
    delay(500);
  }
}

/* Ultrasonic senor */
int ultrasonic_dist(){
  //Obtain readings from ultrasonic sensot
  int dist;
//  Serial.print("Ping: ");
  dist = sonar.ping_cm();  
//  Serial.print(dist);
//  Serial.println("cm");
  //delay(100);
  return dist;
}

/* Getting target coordinate from Raspberry Pi */
struct Coordinate get_target_coor(){
  struct Coordinate coor;
  coor.x = 1.0;
  coor.y = 30.0;
  return coor;
}

float get_distance(struct Coordinate target){
  /* Calculate the Euclidean distance between the car's position with input point, target, of type 'struct Coordinate' */
  Serial.println("Getting distance ...");
  float dist = 0;
  dist = sqrt(sq(target.x)+sq(target.y));
  return dist;
}


void obstacle_avoid(struct Coordinate target){
  float ang = get_angle(target);
  if(ang >= 0){
    turn(1);   // Turn right
    turn(1);   // Turn right
    forward(255); // forward
    forward(255); // forward
  }else{
    turn(-1);    //Turn left
    turn(-1);    //Turn left
    forward(255); // forward
    forward(255); // forward
  }  
}

void obstacle_avoid_fixed(){
  turn(-1);   // Check left side distance
  int left_dist = ultrasonic_dist();
  turn(1);    
  
  turn(1);   // Check right side distance
  int right_dist = ultrasonic_dist();
  turn(-1); // turn back to the middle
  
  if(right_dist < left_dist){
    turn(-1);   // turn left
    turn(-1);
    turn(-1);
    turn(-1);
    forward(255); // forward
    forward(255); // forward
    turn(-1);   // turn left
    forward(255); // forward
    turn(1);
    forward(255); // forward
    forward(255); // forward
    turn(1);
    turn(1);
    forward(255); // forward
    
    turn(1);  // turn back to the middle
   
  }else{
    turn(1);   // turn left
    turn(1);
    turn(1);
    turn(1);
    forward(255); // forward
    forward(255); // forward
    turn(1);   // turn left
    forward(255); // forward
    turn(-1);
    forward(255); // forward
    forward(255); // forward
    turn(-1);
    turn(-1);
    forward(255); // forward

    turn(-1);  // turn back to the middle
   
  }
}

/***Displaying faces on the LED Matrix ***/
void face(int n){
    
    // happy face
byte smile[8]= {B00111100,B01000010,B10100101,B10000001,B10100101,B10011001,B01000010,B00111100};
// neutral face
byte neutral[8]={B00111100, B01000010,B10100101,B10000001,B10111101,B10000001,B01000010,B00111100};
// sad face
byte frown[8]= {B00111100,B01000010,B10100101,B10000001,B10011001,B10100101,B01000010,B00111100};

    if(n == 0){
      printByte(smile);
      delay(1000);
    }
    else if(n ==1 ){
      printByte(neutral);
      delay(1000);
    }
      
     else if(n==2){
      printByte(frown);
      delay(1000);
    }
}


  
 
void printByte(byte sf[])
{
  // Display sad face
  lc.setRow(0,0,sf[0]);
  lc.setRow(0,1,sf[1]);
  lc.setRow(0,2,sf[2]);
  lc.setRow(0,3,sf[3]);
  lc.setRow(0,4,sf[4]);
  lc.setRow(0,5,sf[5]);
  lc.setRow(0,6,sf[6]);
  lc.setRow(0,7,sf[7]);

}



/***Robot falling asleep***/
void naptime(){
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  speakerActivate(4); //"It's nap time!"

  speakerActivate(5); //('Snoring.wav');
  delay(10);
  speakerActivate(6); //displayLCD("Wakeup.wav"); /"Ah that was a good nap! I need coffee"
  return;
}


void shiver(){
  vibration = HIGH;
  //digitalWrite(vibration,HIGH);
  delay(1000);
  vibration = LOW;
  //digitalWrite(vibration,LOW);
 }

void joke() {
  speakerActivate(5);
}
  



void speakerActivate(int code){
  noInterrupts();
  if (SD.begin(SD_Chipselect)){return ;}
   if(code >= 0 && code<=9){
    char file[ ] = {String(code).concat(".wav")};
    tmrpcm.play(file[0]);
    }
  interrupts();
  return;
}
    


void loop()
{
  //Obtain readings from ultrasonic sensot
  face(1);
  /*
  Serial.print("Ping: ");
  dist = sonar.ping_cm();  
  Serial.print(dist);
  Serial.println("cm");
  delay(100);
  head();
  forward();
  */
  
 struct Coordinate target = get_target_coor();

   float target_dist = get_distance(target);
  Serial.print("Distance to target is ");
  Serial.print(target_dist);
  Serial.println(" cm");
  int ultra_dist = ultrasonic_dist();
  Serial.print("Distance to obstacle is ");
  Serial.print(ultra_dist);
  Serial.println(" cm");

  // If too close to any object(wither target/obstacle), < 20cm, STOP, and decide for the next move.
  if(ultra_dist <= 20){
    Stop();
    if(abs(ultra_dist-target_dist) >= 5){  // The object at front is NOT the target
       obstacle_avoid(target);
//       obstacle_avoid_fixed();
       //Because we are updating for a long time, need to refresh parameters
       target = get_target_coor();
       target_dist = get_distance(target);
       ultra_dist = ultrasonic_dist();
    }else{
      // The object at front IS the target
      while(target_dist <= 20){
        // wait until the target moves away
        //ultra_dist = ultrasonic_dist();
        target = get_target_coor();
        target_dist = get_distance(target);
        Serial.print("target distance is ");
        Serial.println(target_dist);
        Stop();
      }
    }
  }

  dir_update();
 if(get_distance(get_target_coor()) >= 30 && ultrasonic_dist() >= 20){
    forward(255);   // forward takes in a int in range (0,255) for PWM
    Serial.println("all clear, move forward to catch target!");
  }else{
    Stop();
  }

  

  
  if(Serial.available() > 0){
    
    incomingByte = Serial.read();
    switch((char)incomingByte){
     case 0:
      speakerActivate(0); //different user
      break;
     case 1:
      speakerActivate(0); //different user
      break;

     default:
       break;
      } 
    }
    
}
