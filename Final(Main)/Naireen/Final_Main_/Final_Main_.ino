#include <SD.h>
#include <TMRpcm.h>
#define SD_Chipselect 11
#define DHT11_PIN A3

//#include <LedControl.h>
#include <Servo.h>
#include <NewPing.h>
#include <LedControl.h>


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

char dataString[50] = {0};

//define other constants
const int max_distance = 300;
char incomingByte;
int DIN = 0;   //dot matrix display 
int CLK = 1;
int CS = 2;
int maxInUse = 1;
// variables to use later on
int dist;
struct Coordinate {int x; int y;};

// create instances of servo and ping class
NewPing sonar(trigPin, echoPin, max_distance);

LedControl lc=LedControl(DIN,CLK,CS,1);

void isLessThanNaireensBag(){
return true;
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(vibration,OUTPUT);
  pinMode(M1,OUTPUT);
  pinMode(M2,OUTPUT);
  pinMode(4, OUTPUT); 
  pinMode(tempSen, INPUT);
  
  
  lc.shutdown(0,false);       //The MAX72XX is in power-saving mode on startup
  lc.setIntensity(0,8);      // Set the brightness to maximum value
  lc.clearDisplay(0);
  
  
  serialread();
}


/* Direction control functions */
float get_angle(struct Coordinate target){
  float ang;   //relative angle between the car heading direction and target direction
               //Netative: on the left of heading direction; Positive: on the right of heading direction
  ang = atan(target.x/target.y)/3.1415926*180;
  return ang;        
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
  int dist;
  dist = sonar.ping_cm();  
  return dist;
}

/* Getting target coordinate from Raspberry Pi */
struct Coordinate get_target_coor(int* master){
  struct Coordinate coor;
  coor.x = master[3];
  coor.y = master[5];
  return coor;
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
  
  byte sleep[8] = {B00111100,B01000010,B10100101,B10000001,B10000001,B10111101,B01000010,B00111100};

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
    
    else if(n==4){
      printByte(sleep);
      delay(1000);
    }
}

//
//int returnTemp()
//{
//  //#include <dht.h>
//  dht DHT;
//  int chk = DHT.read11(DHT11_PIN);
//  int temp = DHT.temperature;
//  return temp;   
//}

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
  //speakerActivate(4); //"It's nap time!"

  //speakerActivate(5); //('Snoring.wav');
  face(4);
  delay(10);
  //speakerActivate(6); //displayLCD("Wakeup.wav"); /"Ah that was a good nap! I need coffee"
  face(0);
  return;
}


void shiver(){
  vibration = HIGH;
  //digitalWrite(vibration,HIGH);
  delay(1000);
  vibration = LOW;
  //digitalWrite(vibration,LOW);
 }

    
int serialread(){
    byte incomingByte = Serial.read();
    int value = (int)incomingByte;
    return value;
}

void dir_update(int * info){
  /* This function performs turning angle calculation, and make a turn. */
  struct Coordinate target = get_target_coor(info);
  int target_dist;
  float ang = get_angle(target);

  if(ang >= 5.0){
    while(ang >= 5.0){
      target = get_target_coor(info);
      target_dist = info[5];
      ang = get_angle(target);
      turn(-1);
    }
  }
  else if(ang <= -5.0){
    while(ang >= 5.0){
      target = get_target_coor(info);
      target_dist = info[5];
      ang = get_angle(target);
      turn(1);
    }
  }
  else{
    forward(255);
  }
}

void follow(){
  int* info = getPi();
  int ultra_dist;
  int distance;
  struct Coordinate target = get_target_coor(info);
  
  while(info[2] == 1){    //While follow command is true
      distance = info[5];
      ultra_dist = ultrasonic_dist();
      // If too close to any object(wither target/obstacle), < 20cm, STOP, and decide for the next move.
      if(ultra_dist <= 20){
        Stop();
        if(abs(ultra_dist - distance) >= 5){  // The object at front is NOT the target
           obstacle_avoid(target);
        }else{
          // The object at front IS the target
          while(distance<= 50){ // need to decide threshold
              // wait until the target moves away
              //ultra_dist = ultrasonic_dist();
              info = getPi();
              distance = info[5];
              Stop();
          }
        }
      }
    
      // Update function
      info = getPi();
      ultra_dist = ultrasonic_dist();
    
      if(info[5] >= 30 && ultra_dist >= 20){
        dir_update(info); // Do a turn or move for one tiny step
        forward(255);   // forward takes in a int in range (0,255) for PWM
      }else{
        Stop();
      }
      // Update function
      int* info = getPi();
  }
}

int* getPi(){
   int value[6] = {0,0,0,0,0,0};
  
  value[0] = serialread();  // user
  value[1] = serialread(); // facedetected
  value[2] = serialread();  // follow command
  value[3] = serialread();  // x offset
  value[4] = serialread(); // y offset
  value[5] = serialread();  // distance
  return value;
}

void hi_Logan(){
  face(0);
  //speakerActivate(); //('hi_logan.wav');
  return;
}

void loop()
{
  
  face(0);
  // Begin serial communication to get values from r pi
  
  // struct Coordinate target;
  int* master = getPi();
  
  //if there is something to track
  if (master[2] == 1){
    follow();
  }
  
  //if there is a face detected, and its Logan's face
  if(master[1] == 1 && master[0] ==1){
  //call function to say "Hi Logan"  
  }
  
//  int temp = returnTemp();
//  if(temp < 15){
//    shiver();
//  }
//  else{
//  naptime();
//  }
}
