#include <Arduino.h>
#include <MKRWAN.h>
#include <DFRobot_SHT3x.h>
#include <ArduinoLowPower.h>

DFRobot_SHT3x sht3x;
#define freqPlan TTN_FP_EU868
float Temp_Buffer = 0;
long lastupload = 0;
long interval = 30000;

LoRaModem modem;
const char *appEui = "0000000000000000";
const char *appKey = "D5576BE41B263788FD3A89E80BAEB979";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
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
   // while (1) {}
  }
 while (sht3x.begin() != 0) {
    Serial.println("Failed to initialize the chip, please confirm the chip connection");
    delay(1000);
  }

  /**
   * readSerialNumber Read the serial number of the chip
   * @return Return 32-digit serial number
   */
  Serial.print("chip serial number: ");
  Serial.println(sht3x.readSerialNumber());
  /**
   * softReset Send command resets via IIC, enter the chip's default mode single-measure mode, 
   * turn off the heater, and clear the alert of the ALERT pin.
   * @return Read the status register to determine whether the command was executed successfully, 
   * and return true indicates success.
   */
   if(!sht3x.softReset()){
     Serial.println("Failed to reset the chip");
   }

  /**
   * pinReset Reset through the chip's reset pin, enter the chip's default mode single-measure mode, 
   * turn off the heater, and clear the alert of the ALERT pin.
   * @return The status register has a data bit that detects whether the chip has been reset, 
   * and return true indicates success.
   * @note When using this API, the reset pin of the chip nRESET should be connected to RST (default to pin4) of arduino.
   */
  //if(!sht3x.pinReset()){
    //Serial.println("Failed to reset the chip");
  //}

  /**
   * heaterEnable() Turn on the heater inside the chip so that the sensor can have accurate humidity data even in humid environment.
   * @return Read the status register to determine whether the command was executed successfully, and return true indicates success.
   * @NOTE Heaters should be used in wet environment, and other cases of use will result in incorrect readings.
   */
  //if(!sht3x.heaterEnable()){
    // Serial.println("Failed to turn on the heater");
  //}
  /**
   * startPeriodicMode Enter cycle measurement mode and set repeatability and read frequency.
   * @param measureFreq Read the eMeasureFrequency_t data frequency.
   * @note  Selectable parameters:
               eMeasureFreq_Hz5,   /**the chip collects data in every 2s
               eMeasureFreq_1Hz,   /**the chip collects data in every 1s 
               eMeasureFreq_2Hz,   /**the chip collects data in every 0.5s 
               eMeasureFreq_4Hz,   /**the chip collects data in every 0.25s 
               eMeasureFreq_10Hz   /**the chip collects data in every 0.1s 
   * @param repeatability Read the repeatability of temperature and humidity data, the default parameter is eRepeatability_High.
   * @note  Optional parameters:
               eRepeatability_High /**In high repeatability mode, the humidity repeatability is 0.10%RH, the temperature repeatability is 0.06°C
               eRepeatability_Medium,/**In medium repeatability mode, the humidity repeatability is 0.15%RH, the temperature repeatability is 0.12°C.
               eRepeatability_Low, /**In low repeatability mode, the humidity repeatability is0.25%RH, the temperature repeatability is 0.24°C
   * @return Read the status of the register to determine whether the command was executed successfully, and return true indicates success
   */          
  if(!sht3x.startPeriodicMode(sht3x.eMeasureFreq_1Hz)){
    Serial.println("Failed to enter the periodic mode");
  }
  Serial.println("------------------Read data in cycle measurement mode-----------------------");
}

void loop() {

  float celsius = sht3x.getTemperatureC();

  // Multiply integer values by 100 to preserve precision (divide by 100 on the server)
  uint16_t temperature = round(100 * celsius);

  // Split both words (16 bits) into 2 bytes of 8
  byte payload[2];
  payload[0] = highByte(temperature);
  payload[1] = lowByte(temperature);

    delay(100);
  if(millis() > 10000 && millis() < 10200){
    /**
     * stopPeriodicMode() Exit from the cycle read data
     * @return Read the status of the register to determine whether the command was executed successfully, 
     * and return true indicates success.
     */
    sht3x.stopPeriodicMode();
    Serial.println("Exited from the cycle measurement mode, enter the single measurement mode");
  }

  
// below is used to set the sending interval, ttn will crash if send message too frequently

 
    int err;
    modem.beginPacket();
    modem.write(payload, sizeof(payload));
    err = modem.endPacket(true);
    if (err > 0) {
      Serial.println("Message sent correctly!");
    } else {
      Serial.println("Error sending message :(");

    }
  
LowPower.deepSleep(900000);
  
}
