#include <Arduino.h>
#include <MKRWAN.h>
#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#define freqPlan TTN_FP_EU868



ClosedCube_HDC1080 hdc1080;
float Temp_Buffer = 0;
long lastupload = 0;
long interval = 30000;
float s = 0;
float s1 = 0;
float s2 = 0;
float s3 = 0;
float s4 = 0;
float s5 = 0;
float s6 = 0;
float averagetemperature = 0;

LoRaModem modem;
const char *appEui = "0000000000000000";
const char *appKey = "2C884DA2F4B6666092421CD06B4DD482";

void setup() {

  Serial.begin(115200);
  delay(10000);
  Serial.println("ClosedCube HDC1080 Measurement Resolutions Arduino Test");

  hdc1080.begin(0x40);
  while (!Serial);
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());


  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }




}

void loop() {

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
  averagetemperature = (s1 + s2 + s3 + s4 + s5 + s6) / 6;
  Serial.println(averagetemperature);
  Serial.println("");
  delay(2000);

  
 float celsius = averagetemperature;
// Multiply integer values by 100 to preserve precision (divide by 100 on the server)
  uint16_t temperature = round(100 * averagetemperature);

  // Split both words (16 bits) into 2 bytes of 8
  byte payload[2];
  payload[0] = highByte(temperature);
  payload[1] = lowByte(temperature);

  // below is used to set the sending interval, ttn will crash if send message too frequently

  if (millis() - lastupload > interval && temperature > 0) {
    int err;
    modem.beginPacket();
    modem.write(payload, sizeof(payload));
    err = modem.endPacket(true);
    if (err > 0) {
      Serial.println("Message sent correctly!");
    } else {
      Serial.println("Error sending message :(");

    }
   lastupload = millis();

  }
   delay(1000);

}

// below defines the function to read temperature

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
