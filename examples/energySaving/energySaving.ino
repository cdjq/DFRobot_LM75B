/*!
 * @file energySaving.ino
 * @brief Enter energy saving mode after the temperature is read; Back up to normal mode and continue to read temperature after 20s, and
 * @n enter energy saving mode again, and so on. 
 *
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-07-30
 * @get from https://www.dfrobot.com
 * @url https://github.com/DFRobot/DFRobot_LM75B
 */
#include <DFRobot_LM75B.h>
/*!
 * @brief Constructor
 * @param pWire I2C bus pointer object, default Wire
 * @param addr 7-bits I2C address, the address value is decided by the first three bits.
 * @n Value (0x48/0x49/0x4A/0x4B/0x4C/0x4D/0x4E/0x4F) default: 0x48
 * @n IIC address is formed as the chart below:
 *   6  5  4  3  2  1   0
     1  0  0  1  A2 A1  A0
 * @n The definition of address is shown below, change the address via jumper: default 0x48
     1  0  0  1  | A2 A1 A0
     1  0  0  1  | 1  1  1       0x4F
     1  0  0  1  | 1  1  0       0x4E
     1  0  0  1  | 1  0  1       0x4D
     1  0  0  1  | 1  0  0       0x4C
     1  0  0  1  | 0  1  1       0x4B
     1  0  0  1  | 0  1  0       0x5A
     1  0  0  1  | 0  0  1       0x49
     1  0  0  1  | 0  0  0       0x48
*/
//Use this constructor function to define software IIC and change chip address. 
//DFRobot_LM75B lm75b(&Wire, 0x48);
DFRobot_LM75B lm75b; 

void setup(void) {
  Serial.begin(115200);
  //Detect if IIC communication works properly
  while (lm75b.begin() != 0) {
    Serial.println("IIC init failed, please check if the connection is correct?");
    delay(1000);
  }
}

void loop(void) {
  /*!
   Set chip working mode 
   ShutDownMode value:
   eNormal  In this mode, data acquisition cycle is 100ms, in which 10ms is used for data conversion and it needs 300uA current.
            The rest is in idle state and it needs 10uA.
   eShutdown In this mode, data acquisition stops, but IIC communication and register works normally.
  */
  lm75b.setShutDownMode(/*ShutDownMode=*/lm75b.eNormal);
  //a certian data conversion cycle is needed
  delay(200);
  Serial.println("Chip enters normal mode, read temperature........");
  Serial.print("ambient temperature (°C): ");
  Serial.print(/*Celsius temperature=*/lm75b.getTemperatureC());
  Serial.println("°C");
  Serial.print("ambient temperature(°F) ");
  Serial.print(/*Fahrenheit=*/lm75b.getTemperatureF());
  Serial.println("°F");
  Serial.println("Chip enters energy saving mode........");
  lm75b.setShutDownMode(/*ShutDownMode=*/lm75b.eShutdown);
  // Read temperature once and enter energy saving mode immediately every 20s. 
  delay(20000);
}
