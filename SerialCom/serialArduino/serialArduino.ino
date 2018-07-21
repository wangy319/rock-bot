char dataString[50] = {0};
int a =0; 
int b = 12;
const int ledPin = 13;
void setup() {
Serial.begin(9600);              //Starting serial communication
pinMode(b,OUTPUT);
}
 /*
void loop() {
  a++;                          // a value increase every loop
  sprintf(dataString,"%02X",a); // convert a value to hexa 
  Serial.println(dataString);   // send the data
  delay(1000);                  // give the loop some break
}
*/
void loop(){
  /*
	if(Serial.available()){
    flash(Serial.read() - '0');
	}
  */
		byte incomingByte = Serial.read();
    //Serial.print(incomingByte);
    //String myString = String(incomingByte);
    //Serial.print(myString);
    //Serial.print("n");
    String on = "on";
    String off = "off";
		 if (incomingByte == 'a'){
			//Serial.print("uh-uh");
      Serial.print((String)incomingByte);
      digitalWrite(b, LOW);
		 }
		 else if (incomingByte == 'b'){
			//Serial.print("yep");
      Serial.print((String)incomingByte);
      digitalWrite(b, HIGH);
		 }
	} 
  
	
 void flash(int n)
{
  for (int i = 0; i < n; i++)
  {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}
