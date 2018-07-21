#include <Servo.h>
#include <NewPing.h>
#include <math.h>

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
struct Coordinate {float x; float y;};

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


/* Excecutable helper functions*/

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
  coor.x = 0;
  coor.y = 50.0;
  return coor;
}

float get_distance(struct Coordinate target){
  /* Calculate the Euclidean distance between the car's position with input point, target, of type 'struct Coordinate' */
  Serial.println("Getting distance ...");
  float dist = 0;
  dist = sqrt(sq(target.x)+sq(target.y));
  delay(100);
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

void loop()
{
  // Get target coordinate from Raspberry Pi
  struct Coordinate target = get_target_coor();

  /* if ultrasonic_diststance <= 20, stop moving, excecute obstacle avoidance functions, may go away further, 
   * then continue to excecute the following moves*/
  float target_dist = get_distance(target);
  Serial.print("Distance to target is ");
  Serial.print(target_dist);
  Serial.println(" cm");
  int ultra_dist = ultrasonic_dist();
  Serial.print("Distance to obstacle is ");
  Serial.print(ultra_dist);
  Serial.println(" cm");
  
  // If too close to any object(wither target/obstacle), < 20cm, STOP, and decide for the next move.
  if(ultra_dist <= 30){
    Stop();
    if(abs(ultra_dist-target_dist) >= 10){  // The object at front is NOT the target
       obstacle_avoid();
       //Because we are updating for a long time, need to refresh parameters
       target = get_target_coor();
       target_dist = get_distance(target);
       ultra_dist = ultrasonic_dist();
    }else{
      // The object at front IS the target
      while(target_dist <= 30){
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

  // Update direction by doing a turning move
  dir_update();
  // Follow target by moving forward
  if(get_distance(get_target_coor()) >= 30 && ultrasonic_dist() >= 20){
    forward(255);   // forward takes in a int in range (0,255) for PWM
    delay(200);
    Serial.println("all clear, move forward to catch target!");
  }else{
    Stop();
  }
}

