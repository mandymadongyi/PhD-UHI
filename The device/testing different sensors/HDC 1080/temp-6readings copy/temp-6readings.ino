#include <Arduino.h>
#include <MKRWAN.h>
#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#define freqPlan TTN_FP_EU868



ClosedCube_HDC1080 hdc1080;
float Temp_Buffer = 0;
long lastupload = 0;
long interval = 30000;
float t = 0;
float t1 = 0;
float t2 = 0;
float t3 = 0;
float t4 = 0;
float t5 = 0;
float t6 = 0;


float h = 0;
float h1 = 0;
float h2 = 0;
float h3 = 0;
float h4 = 0;
float h5 = 0;
float h6 = 0;

LoRaModem modem;
const char *appEui = "0000000000000000";
const char *appKey = "A150EA8BE8100C63F32BD84A478DC2D0";

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
  t1 = t;
  h1 = h;
  Serial.println(t1);
  printTandRH(HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_11BIT);
  t2 = t;
  h2 = h;
  Serial.println(t2);
  printTandRH(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_11BIT);
  t3 = t;
  h3 = h;
  Serial.println(t3);
  printTandRH(HDC1080_RESOLUTION_8BIT, HDC1080_RESOLUTION_14BIT);
  t4 = t;
  h4 = h;
  Serial.println(t4);
  printTandRH(HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_14BIT);
  t5 = t;
  h5 = h;
  Serial.println(t5);
  printTandRH(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);
  t6 = t;
  h6 = h;
  Serial.println(t6);
  delay(2000);

  delay(2000);



  // Multiply integer values by 100 to preserve precision (divide by 100 on the server)
  uint16_t temp1 = round(100 * t1);
  uint16_t humidity1 = round(100 * h1);

  uint16_t temp2 = round(100 * t2);
  uint16_t humidity2 = round(100 * h2);

  uint16_t temp3 = round(100 * t3);
  uint16_t humidity3 = round(100 * h3);

  uint16_t temp4 = round(100 * t4);
  uint16_t humidity4 = round(100 * h4);

  uint16_t temp5 = round(100 * t5);
  uint16_t humidity5 = round(100 * h5);

  uint16_t temp6 = round(100 * t6);
  uint16_t humidity6 = round(100 * h6);

  // Split both words (16 bits) into 2 bytes of 8
  byte payload[24];
  payload[0] = highByte(temp1);
  payload[1] = lowByte(temp1);
  payload[2] = highByte(humidity1);
  payload[3] = lowByte(humidity1);

  payload[4] = highByte(temp2);
  payload[5] = lowByte(temp2);
  payload[6] = highByte(humidity2);
  payload[7] = lowByte(humidity2);

  payload[8] = highByte(temp3);
  payload[9] = lowByte(temp3);
  payload[10] = highByte(humidity3);
  payload[11] = lowByte(humidity3);

  payload[12] = highByte(temp4);
  payload[13] = lowByte(temp4);
  payload[14] = highByte(humidity4);
  payload[15] = lowByte(humidity4);

  payload[16] = highByte(temp5);
  payload[17] = lowByte(temp5);
  payload[18] = highByte(humidity5);
  payload[19] = lowByte(humidity5);

  payload[20] = highByte(temp6);
  payload[21] = lowByte(temp6);
  payload[22] = highByte(humidity6);
  payload[23] = lowByte(humidity6);

  
  // below is used to set the sending interval, ttn will crash if send message too frequently

  if (millis() - lastupload > interval && temp1 > 0) {
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
