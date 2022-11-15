#include <Arduino.h>
#include <MKRWAN.h>
#include <Adafruit_MAX31865.h>
Adafruit_MAX31865 thermo = Adafruit_MAX31865(10, 11, 12, 13);
#define freqPlan TTN_FP_EU868
#define RREF      430.0
#define RNOMINAL  100.0

float Temp_Buffer = 0;
long lastupload = 0;
long interval = 30000;

LoRaModem modem;
const char *appEui = "0000000000000000";
const char *appKey = "0D8E0CA7306FFE2F37A04983794891D6";

void setup() {
  Serial.begin(115200);
   while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
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

  delay(10000);
  Serial.println("Adafruit MAX31865 PT100 Sensor Test!");

  thermo.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
}


void loop() {

  uint16_t temperature = round(100 * thermo.temperature(RNOMINAL, RREF));

 

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
