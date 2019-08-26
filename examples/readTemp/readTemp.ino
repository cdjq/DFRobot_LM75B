/*!
 * @file readTemp.ino
 * @brief Read ambient temperature
 * @n Experiment phenomenon: read chip setting before starting, and then read temperature once per second.
 * @n Chip collects temperature data every 100ms. 
 * @n Reading temperature data does not affect the coversion and acquisition in progress during the read operation.
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
 * @n Value (0x48/0x49/0x4A/0x4B/0x4C/0x4D/0x4E/0x4F) default 0x48
 * @n IIC address is formed as the chart below
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
  //Chip will be reset when repowered. 
  Serial.println("**--------------------the chip set----------------------**");
  Serial.print("Chip working mode: ");
  /**
    Normal mode: chip works normally, and it collects temperature data every 100ms and store data to temperature register. 
    Shutdown mode: data conversion disabled, but register read/write operation can be performed.
  */
  Serial.print(lm75b.getShutDownMode());
  Serial.println(" (0:nomal mode/1：shutdown mode)");
  Serial.print("OS mode: ");
  /**
    Comparator mode: OS output becomes active when the temperature exceeds the Tos, and is reset when the temp drops below Thyst.
    Interrupt mode: OS output is first activated only when the Temp exceeds the Tos and generates a pulse signal, and then remains.
    It generates the next pulse signal only when the temperature is below Thyst, and so on. 
  */
  Serial.print(lm75b.getOSMode());
  Serial.println(" (0: comparator mode/1：shutdown mode)");
  /*!
    The OS output active state can be selected as HIGH or LOW by programming bit B2
    (OS_POL) of register Conf
     eActive_LOW = 0,  <OS active state is LOW in this mode>
     eActive_HIGH = 1  <OS active state is HIGH in this mode>
    When temperature is more than threshold temperature, OS output is active state (defalut: LOW).
  */
  Serial.print("OS Polarity: ");
  Serial.print(lm75b.getOSPolarityMode());
  Serial.println(" (0:active LOW/1：active HIGH)");
  /*!
    OS generates interrupt only when there is enough number of queue faults.
    Number of faults in the queue: Each time the temperature in the temperature register completes conversion, it will automatically
    be compared with threshold and hystersis temperature. 
    eValue1=1, if one temperature value is more than threshold value, OS output active state;
    eValue2=2, if two successive temperature values are more than threshold value, OS output active state; 
    eValue3=4, if four successive temperature values are more than threshold value, OS output active state; 
    eValue4=6, if six successive temperature values are more than threshold value, OS output active state.
   */
  Serial.print("OS fault queue: ");
  Serial.print(lm75b.getQueueValue());
  Serial.println(" (The number of times that ambient temperature consecutively exceeds threshold to make OS output become active state.)");
  //User-defined value, OS state changes when ambient temperature exceeds this value.
  /*getTosC Get the threshold value stored in Tos register. 
  */
  Serial.print("Threshold (degree Celsius): ");
  /**
   * @brief Get threshold temperature(Tos:Overtemperature shutdown).
   * @return Return temperature value, unit: °C
   * @n Detection range: -55°C to +125°C.
   */
  Serial.print(lm75b.getTosC());
  Serial.println("°C");
  /**
   * @brief Get threshold temperature(Tos:Overtemperature shutdown).
   * @return Return temperature value, unit: °F 
   * @n Detection range: -67°F to +257°F.
   */
  //Serial.print("Threshold (Fahrenheit): ");
  //Serial.print(lm75b.getTosF());
  //Serial.println("°F");
  /*!
    getHysteresisC Get hysteresis temperature stored in Thyst register
    User-defined hysteresis temperature; delay the level jump of OS: OS level will jump when the
    ambient temperature is less than hysteresis value instead of threshold value.
    Effect: when the temperature is more than threshold temperature, OS pin becomes active(default LOW)
    When the temperature is less than threshold temperature, OS pin will not back to the normal 
    state(default HIGH) until the temperature value is less than the hysteresis tempreature.
  */
  Serial.print("Hysteresis temperature (degree Celsius): ");
  Serial.print(lm75b.getHysteresisC());
  Serial.println("°C");
  /**
   * @brief Get hysteresis temperature (User-defined temperature, ≤ Threshold)..
   * @return Return temperature, unit: °F 
   * @n Detection range: -67°F to +257°F.
   */
  //Serial.print("Hysteresis temperature (Fahrenheit): ");
  //Serial.print(lm75b.getHysteresisF());
  //Serial.println("°F");
  Serial.println("**-----------------------------------------------------**");
}

void loop(void) {
  Serial.print("Ambient temperature (°C): ");
  Serial.print(/*degree Celsius=*/lm75b.getTemperatureC());
  Serial.println("°C");
  Serial.print("Ambient temperature (°F): ");
  Serial.print(/*Fahrenheit=*/lm75b.getTemperatureF());
  Serial.println("°F");
  Serial.println("");
  // delay should be over 100ms;
  delay(1000);
}
