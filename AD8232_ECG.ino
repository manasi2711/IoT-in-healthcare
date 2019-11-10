#include <Arduino_JSON.h>

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
int ecg;
SoftwareSerial s(5,6);

void setup() {
// initialize the serial communication:
Serial.begin(9600);
s.begin(9600);
pinMode(10, INPUT); // Setup for leads off detection LO +
pinMode(11, INPUT); // Setup for leads off detection LO -

}

//creating JSON class object
StaticJsonBuffer<1000> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

void loop() {

if((digitalRead(10) == 1)||(digitalRead(11) == 1)){
Serial.println('!');

}
else{
// send the value of analog input 0:
Serial.println(analogRead(A0));

}
//Wait for a bit to keep serial data from saturating
ecg = analogRead(A0);
root["ecg"] = ecg;

if(s.available()>0)
{
  root.printTo(s);
 delay(100);
}
}
