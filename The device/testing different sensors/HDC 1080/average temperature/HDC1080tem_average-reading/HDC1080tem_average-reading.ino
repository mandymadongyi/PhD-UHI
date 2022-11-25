/**************************************************************************************
  The original code takes 6 measurements at one time,
  this code calculate the average value of the 6 measurements to prepare the final result to transmit by LoRa.
  Mandy 2022/11/01
**************************************************************************************/

#include <Wire.h>
#include "ClosedCube_HDC1080.h"

ClosedCube_HDC1080 hdc1080;
float s = 0;
float s1 = 0;
float s2 = 0;
float s3 = 0;
float s4 = 0;
float s5 = 0;
float s6 = 0;
float temperature = 0;

void setup()
{
  Serial.begin(115200);
  delay(10000);
  Serial.println("ClosedCube HDC1080 Measurement Resolutions Arduino Test");

  hdc1080.begin(0x40);

}

void loop()
{
  printTandRH(HDC1080_RESOLUTION_8BIT, HDC1080_RESOLUTION_11BIT);
  s1 = s;
  Serial.println(s1);
  printTandRH(HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_11BIT);
  s2 = s;
  Serial.println(s2);
  printTandRH(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_11BIT);
  s3 = s;
  Serial.println(s3);
  printTandRH(HDC1080_RESOLUTION_8BIT, HDC1080_RESOLUTION_14BIT);
  s4 = s;
  Serial.println(s4);
  printTandRH(HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_14BIT);
  s5 = s;
  Serial.println(s5);
  printTandRH(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);
  s6 = s;
  Serial.println(s6);
  delay(2000);
  temperature = (s1 + s2 + s3 + s4 + s5 + s6) / 6;
  Serial.println(temperature);
  Serial.println("");
  delay(2000);
}

void printTandRH(HDC1080_MeasurementResolution humidity, HDC1080_MeasurementResolution temperature) {
  hdc1080.setResolution(humidity, temperature);

  HDC1080_Registers reg = hdc1080.readRegister();
  printRegister(reg);
  s = hdc1080.readTemperature();
  Serial.print("T=");
  Serial.print(s);
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
