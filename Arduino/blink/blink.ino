#include <dht.h>
#define DHT11_PIN 7

const int ledPin = 13;
dht DHT;

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  //Serial.println("Hello Pi");
  if (Serial.available())
  {
     flash(Serial.read() - '0');
  }

  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  
  delay(500);
  flash(1);
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
