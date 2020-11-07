// this example is public domain. enjoy!
// https://learn.adafruit.com/thermocouple/

#include "max6675.h"

#define thermoDO 4
#define thermoCS 5
#define thermoCLK 6

int vccPin = 3;
int gndPin = 2;

void setup() {
  Serial.begin(9600);
  // use Arduino pins 
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);

  MAX6675<thermoCLK, thermoCS, thermoDO>::init();

  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp

   Serial.print("C = "); 
   Serial.println(MAX6675<thermoCLK, thermoCS, thermoDO>::readCelsius());
   Serial.print("F = ");
   Serial.println(MAX6675<thermoCLK, thermoCS, thermoDO>::readFahrenheit());

   // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
   delay(1000);
}
