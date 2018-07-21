#include <Wire.h>
#include <SD.h>
#include <TMRpcm.h>
#define SD_Chipselect 11

TMRpcm tmrpcm;
int x =0;
void setup() {
  tmrpcm.speakerPin = 9;
  tmrpcm.volume(1);
}

void speakerActivate(int code){
  noInterrupts();
  if (SD.begin(SD_Chipselect)){return ;}
   if(code >= 0 && code<=9){
    char* file[ ] = {String(code).concat(".wav")};
    tmrpcm.play(file[0]);
    }
  interrupts();
  return;
}

void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
}
void loop() {
  /*
  Wire.begin(9); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  */
  if (x==0){
    //speakerActivate(0);
    char* file[ ] = {String(0).concat(".wav")};
    tmrpcm.play(file[0]);
  }

  if(x==1){
    speakerActivate(1);
  }
  delay(3000);
}





