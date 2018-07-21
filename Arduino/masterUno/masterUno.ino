#include <Wire.h>
int sound = 0;

void setup() {
  // Start the I2C Bus as Master
  Wire.begin(); 
}
void loop() {
  
  sendVoice(sound);
  
}

void sendVoice(int x){
  Wire.beginTransmission(9);
  Wire.write(x);
  Wire.endTransmission();
  return;
}
