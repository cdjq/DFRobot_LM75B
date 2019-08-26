# DFRobot_LM75B
The LM75B is a temperature-to-digital converter using an on-chip band gap temperature sensor and Sigma-Delta A-to-D conversion technique with an overtemperature output. It can be used to moniter the ambient temperature. The device also includes an open-drain output (OS) which becomes active when the temperature exceeds the programmed limits. The LM75B can communicate with controllers via a 2-wire serial I2C-bus interface. There are three selectable logic addreses pins so that eight devices can be connected on the same bus without address conflict. The LM75B can be configured for different operation conditions. It can be set in normal mode to periodically monitor the ambient temperature, or in shutdown mode to minimize power consumption. <br>

  * Resolution: 0.125°C <br>
  * Accuracy: ±2 °C <br>
  * Power Consumption: operating current typically 200uA, power down mode 1.0uA <br>
  * Operating Voltage: 2.8V～5.5V<br>
![正反面svg效果图](https://github.com/ouki-wang/DFRobot_Sensor/raw/master/resources/images/SEN0245svg1.png)

## Product Link（链接到英文商城）
    SKU：LM75B 温度传感器
   
## Table of Contents

* [Summary](#summary)
* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary

   1. Read ambient temperature and the value will be printed in ℃ and ℉. <br>
   2. Chip setting of serial print (chip operaton mode, OS polarity, OS output mode, the number of faults set in the queue, temperature threshold, hysteresis temperature). <br>
   3. A high temperature alarm will be shown on the serial port when the temperature exceeds user-defined temperature threshold.<br>
   4. Use as a monitor device of thermostat to keep the temperature at a user-defined threshold point.<br>

## Installation

To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

## Methods

```C++
    
/*!
 * @brief Constructor
 * @param pWire I2C bus pointer object, default Wire
 * @param addr 7-bits I2C address, the address value is decided by the first three bits.
 * @n Value (0x48/0x49/0x4A/0x4B/0x4C/0x4D/0x4E/0x4F), default to 0x48
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
DFRobot_LM75B(TwoWire *pWire = &Wire, uint8_t address = 0x48); 

/**
 * @brief Init funciton
 * @return Return 0 if the initialization succeeds, otherwise return non-zero and error code.
 */
int begin();

/**
 * @brief Get ambient temperature value 
 * @return Return ambient temperature value, unit: °F
 * @n Detection range: -67°F to +257°F
 */
float getTemperatureF();
/**
 * @brief Get ambient temperature value
 * @return Return ambient temperature, unit: °C
 * @n Detection range: -55°C to +125°C
 */
float getTemperatureC();

/**
 * @brief Get threshold temperature (Tos:Overtemperature shutdown).
 * @return Return temperature, unit: °C
 * @n Detection range: -55°C to +125°C
 */
float getTosC(void );

/**
 * @brief Get threshold temperature (Tos:Overtemperature shutdown).
 * @return Return temperature, unit: °F
 * @n Detection range: -67°F to +257°F
 */
float getTosF(void );

/**
 * @brief Get hysteresis temperature (user-defined temperature point, ≤ threshold temperature)..
 * @return Return temperature, unit: °C
 * @n Detection range: -55°C to +125°C.
 */
float getHysteresisC();

/**
 * @brief Get hysteresis temperature (user-defined temperature point,  ≤ threshold temperature)..
 * @return Return temperature, unit: °F 
 * @n Detection range: -67°F to +257°F.
 */
float getHysteresisF();

/**
 * @brief Set threshold temperature (User-defined temperature point)
 * @param Tos Temperature, unit: °C, Tos% 0.5 == 0 ；
 * @n Detection range: -55°C to +125°C
 */
void setTosC(float Tos);
/**
 * @brief Set threshold temperature (User-defined temperature point)
 * @param Tos Temperature, unit: °F, Tos% 0.5 == 0 ；
 * @n Detection range: -67°F to +257°F
 */
void setTosF(float TosF);
/**
 * @brief User-defined hysteresis temperature
 * @param Thyst Temperature, unit: °C, Thyst%0.5 == 0;
 * @n Detection range: -55°C to +125°C, Thyst must be less than Tos; 
 * @n User-defined hysteresis temperature; delay the level jump of OS: OS level will jump when the
 * @n ambient temperature is less than hysteresis value instead of threshold value. 
 * @n Effect: when the temperature is more than threshold temperature, OS pin becomes active(default LOW) 
 * @n         When the temperature is less than threshold temperature, OS pin will not back to the normal 
 * @n state(default HIGH) until the temperature value is less than the hysteresis tempreature.  
 */
void setHysteresisC(float Thyst);

/**
 * @brief User-defined hysteresis temperature 
 * @param Thyst Temperature, unit: °F, Thyst%0.5 == 0 ；
 * @n Detection range: -67°F to +257°F, Thyst must be less than Tos
 */
void setHysteresisF(float ThystF);

/**
 * @brief Get fault queue value
 * @return Return fault queue value 
 */
eQueueValue_t getQueueValue();

/**
 * @brief Set the value of fault queue
 * @param value eQueueValue_t type, represents the number of faluts set in the queue.
 * @n Set value to ：
 * @n Each time the temperature in the temperature register completes conversion, it will be automatically
 * @n compared with threshold and hysteresis temperature. 
 * @n eValue1, if one temperature value is more than threshold value, OS output active state; 
 * @n eValue2, if two successive temperature values are more than threshold value, OS output active state; 
 * @n eValue3, if four successive temperature values are more than threshold value, OS output active state; 
 * @n eValue4, if six successive temperature values are more than threshold value, OS output active state.
 */
void setQueueValue(eQueueValue_t value);

/**
 * @brief Get chip working mode
 * @return 0 or 1 
 * @n 0：Normal(normal mode)
 * @n 1：interrupt(shut down mode)
 */
eShutDownMode_t getShutDownMode();

/**
 * @brief Set chip working mode
 * @param ShutDownMode eQueueValue_t type, represents chip working mode
 */
void setShutDownMode(eShutDownMode_t ShutDownMode);

/**
 * @brief Get active state of OS pin (HIGH/LOW) 
 * @return 0 or 1 
 * @n 0：LOW(active LOW)
 * @n 1：HIGH(active HIGH)
 */
eOSPolarityMode_t getOSPolarityMode();
/**
 * @brief Set active state of OS pin to HIGH/LOW
 * @param polarityMode eOSPolarityMode_t type, represents polarity of OS pin
 */
void setOSPolarityMode(eOSPolarityMode_t polarityMode);

/**
 * @brief Get OS pin working mode
 * @return 0 or 1 
 * @n 0：OS comparator(comparator mode)
 * @n 1：OS interrupt(interrupt mode)
 */
eOSMode_t getOSMode();

/**
 * @brief Set OS pin working mode 
 * @param OSMode eOSMode_t type, represents OS pin working mode
 */
void setOSMode(eOSMode_t OSMode);
```

## Compatibility

MCU                | Work Well    | Work Wrong   | Untested    | Remarks
------------------ | :----------: | :----------: | :---------: | -----
Arduino Uno        |      √       |              |             | 
Mega2560        |      √       |              |             | 
Leonardo        |      √       |              |             | 
ESP32        |      √       |              |             | 
micro:bit        |      √       |              |             | 


## History

- Data 2019-7-31
- Version V0.1


## Credits

Written by fengli(li.feng@dfrobot.com), 2019.7.31 (Welcome to our [website](https://www.dfrobot.com/))





