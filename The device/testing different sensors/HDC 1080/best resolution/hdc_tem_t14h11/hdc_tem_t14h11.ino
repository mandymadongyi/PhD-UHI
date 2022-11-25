/**************************************************************************************

This is example for ClosedCube HDC1080 Humidity and Temperature Sensor breakout booard

Initial Date: 07-Jun-2017

Hardware connections for Arduino Uno:
VDD to 3.3V DC
SCL to A5
SDA to A4
GND to common ground

Written by AA for ClosedCube

MIT License

**************************************************************************************/

#include <Wire.h>
#include "ClosedCube_HDC1080.h"

ClosedCube_HDC1080 hdc1080;

void setup()
{
  Serial.begin(115200);
  delay(10000);
 

  hdc1080.begin(0x40);

  
}

void loop()
{ 
  
  printTandRH(HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_14BIT);
  
  delay(2000);
}

void printTandRH(HDC1080_MeasurementResolution humidity, HDC1080_MeasurementResolution temperature) {
  hdc1080.setResolution(humidity, temperature);

  HDC1080_Registers reg = hdc1080.readRegister();
  printRegister(reg);

  Serial.print("T=");
  Serial.print(hdc1080.readTemperature());
  Serial.print("C, RH=");
  Serial.print(hdc1080.readHumidity());
  Serial.println("%");
}

void printRegister(HDC1080_Registers reg) {
  Serial.print("Measurement Resolution: T=");
  Serial.print(reg.TemperatureMeasurementResolution, BIN);
  Serial.print(" (0=14 bit, 1=11 bit)");

  Serial.print(" RH=");
  Serial.print(reg.HumidityMeasurementResolution, BIN);
  Serial.println(" (00=14 bit, 01=11 bit, 10=8 bit)");
}
