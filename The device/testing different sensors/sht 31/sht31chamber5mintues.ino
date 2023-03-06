#include <Arduino.h>
#include <MKRWAN.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include <ArduinoLowPower.h>

bool enableHeater = false;
uint8_t loopCnt = 0;

Adafruit_SHT31 sht31 = Adafruit_SHT31();
#define freqPlan TTN_FP_EU868
float Temp_Buffer = 0;


LoRaModem modem;
const char *appEui = "0000000000000000";
const char *appKey = "BC443E3DBDF1E99F6E017C28E33C7A4E";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  //while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)


  //while (!Serial)
    //delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
   // while (1) delay(1);
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");

    
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
   // while (1) {}
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

void loop() {

  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t");
  } else {
    Serial.println("Failed to read temperature");
  }

  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(h);
  } else {
    Serial.println("Failed to read humidity");
  }

  delay(1000);

  // Toggle heater enabled state every 30 seconds
  // An ~3.0 degC temperature increase can be noted when heater is enabled
  if (loopCnt >= 30) {
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    Serial.print("Heater Enabled State: ");
    if (sht31.isHeaterEnabled())
      Serial.println("ENABLED");
    else
      Serial.println("DISABLED");

    loopCnt = 0;
  }
  loopCnt++;

  

  // Multiply integer values by 100 to preserve precision (divide by 100 on the server)
  uint16_t temperature = round(100 * t);

  // Split both words (16 bits) into 2 bytes of 8
  byte payload[2];
  payload[0] = highByte(temperature);
  payload[1] = lowByte(temperature);

  delay(100);


  if (temperature > 0) {
    int err;
    modem.beginPacket();
    modem.write(payload, sizeof(payload));
    err = modem.endPacket(true);


  }

  // here defines the system sleep length


  LowPower.deepSleep(300000);

}
