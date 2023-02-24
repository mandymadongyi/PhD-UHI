#include <Arduino.h>
#include <MKRWAN.h>
#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#include <ArduinoLowPower.h>
#define freqPlan TTN_FP_EU868


ClosedCube_HDC1080 hdc1080;
float t = 0;
float t1 = 0;

float h = 0;
float h1 = 0;

LoRaModem modem;
const char *appEui = "0000000000000000";
const char *appKey = "28ED0868C5E40FD56A96CA1ABBE25398";


void setup() {

  Serial.begin(115200);
  delay(10000);
  Serial.println("ClosedCube HDC1080 Measurement Resolutions Arduino Test");

  hdc1080.begin(0x40);
  //while (!Serial){}

  delay(1000);
  
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    //while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());


  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    //while (1) {}
  }
}

void loop()
{

  printTandRH(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);
  t1 = t;
  h1 = h;
  
  // Multiply integer values by 100 to preserve precision (divide by 100 on the server)
  uint16_t temperature = round(100 * t1);
  uint16_t humidity = round(100 * h1);
  // Split both words (16 bits) into 2 bytes of 8
  byte payload[4];
  payload[0] = highByte(temperature);
  payload[1] = lowByte(temperature);
  payload[2] = highByte(humidity);
  payload[3] = lowByte(humidity);
  
  // below is used to set the sending interval, ttn will crash if send message too frequently

  //if (temperature > 0) {
    int err;
    modem.beginPacket();
    modem.write(payload, sizeof(payload));
    err = modem.endPacket(true);
    if (err > 0) {
      Serial.println("Message sent correctly!");
    } else {
      Serial.println("Error sending message :(");

    }
   


    LowPower.deepSleep(300000);
  //}
}


void printTandRH(HDC1080_MeasurementResolution humidity, HDC1080_MeasurementResolution temperature) {
  hdc1080.setResolution(humidity, temperature);

  HDC1080_Registers reg = hdc1080.readRegister();
  printRegister(reg);
  t = hdc1080.readTemperature();
  h = hdc1080.readHumidity();
  Serial.print("T=");
  Serial.print(t);
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
