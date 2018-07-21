/*

*/
#include <SD.h>
#include <TMRpcm.h>
#define SD_Chipselect 11

//#include <LedControl.h>
#include <Servo.h>
#include <NewPing.h>


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


/***
MaxMatrix m(DIN, CLK, CS, maxInUse);


char smile01[] = {8, 8,
                  B00111100,
                  B01000010,
                  B10010101,
                  B10100001,
                  B10100001,
                  B10010101,
                  B01000010,
                  B00111100
                 };
char smile02[] = {8, 8,
                  B00111100,
                  B01000010,
                  B10010101,
                  B10010001,
                  B10010001,
                  B10010101,
                  B01000010,
                  B00111100
                 }; */

//LedControl lc=LedControl(DIN,CLK,CS,0);


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
	
  
//  lc.shutdown(0,false);       //The MAX72XX is in power-saving mode on startup
//  lc.setIntensity(0,15);      // Set the brightness to maximum value
//  lc.clearDisplay(0);
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

void obstacle_avoid(){
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
/*void face(int n){
    
    byte smile[8]=   {0x3C,0x42,0xA5,0x81,0xA5,0x99,0x42,0x3C};
    byte neutral[8]= {0x3C,0x42,0xA5,0x81,0xBD,0x81,0x42,0x3C};
    byte frown[8]=   {0x3C,0x42,0xA5,0x81,0x99,0xA5,0x42,0x3C};
    
    
    switch(n){
      case '0':
      printByte(smile);
      delay(1000);
      break;
    
      case '1':
      printByte(neutral);
      delay(1000);
      break;
      
      case '2':
      printByte(frown);
      break;
    }
}

*/
void printByte(byte character [])
{
  int i = 0;
  for(i=0;i<8;i++)
  {
//    lc.setRow(0,i,character[i]);
  }
}



/***Robot falling asleep***/
void naptime(){
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  speakerActivate(4); //"It's nap time!"
  //face(2); //animation of closing eyes
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
       obstacle_avoid();
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
		 case '0':
			speakerActivate(0); //different user
		  break;
		 case '1':
			speakerActivate(0); //different user
		  break;

		 default:
		   break;
      } 
    }
}
