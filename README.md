# DFRobot_LM75B
The LM75B is a temperature-to-digital converter using an on-chip band gap temperature sensor and Sigma-Delta A-to-D conversion technique with an overtemperature output. It can be used to moniter the ambient temperature. The device also includes an open-drain output (OS) which becomes active when the temperature exceeds the programmed limits. The LM75B can communicate with controllers via a 2-wire serial I2C-bus interface. There are three selectable logic addreses pins so that eight devices can be connected on the same bus without address conflict. The LM75B can be configured for different operation conditions. It can be set in normal mode to periodically monitor the ambient temperature, or in shutdown mode to minimize power consumption. <br>

  Resolution: 0.125°C <br>
  Accuracy: ±2 °C <br>
  Power Consumption: operating current typically 200uA, power down mode 1.0uA <br>
  Operating Voltage: 2.8V～5.5V<br>
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

   1.Read ambient temperature and the value will be printed in ℃ and ℉. <br>
   2.Chip setting of serial print (chip operaton mode, OS polarity, OS output mode, the number of faults set in the queue, temperature threshold, hysteresis temperature limit). <br>
   3.A high temperature warning will be shown on the serial port when the temperature exceeds user-defined temperature threshold.<br>
   4.Use as a monitor device of thermostat to keep the temperature at a user-defined threshold point.<br>

## Installation

To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

## Methods

```C++
    
/*!
 * @brief Constructor
 * @param pWire I2C bus pointer object, default Wire
 * @param addr 7-bits I2C address, the address value is decided by the first three bits, get value (0x48/0x49/0x4A/0x4B/0x4C/0x4D/0x4E/0x4F) default 0x48
 * @n IIC地址由构成如下图所示
 *   6  5  4  3  2  1   0
     1  0  0  1  A2 A1  A0
 * @n 地址的定义如下表所示,可以通过跳线来改变地址：默认为0x48
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
 * @brief 初始化函数
 * @return 返回0表示初始化成功，返回其他值表示初始化失败，返回错误码
 */
int begin();

/**
 * @brief 获取环境温度值.
 * @return 返回环境温度值，单位是华氏度.
 * @n 可以检测的温度范围是 -67°F 到 +257°F
 */
float getTemperatureF();
/**
 * @brief 获取环境温度值.
 * @return 返回环境温度值，单位是摄氏度.
 * @n 可以检测的温度范围是 -55°C 到 +125°C
 */
float getTemperatureC();

/**
 * @brief 获取阈值温度(Tos:Overtemperature shutdown).
 * @return 返回温度值，单位是摄氏度.
 * @n 温度值范围是 -55°C 到 +125°C.
 */
float getTosC(void );

/**
 * @brief 获取阈值温度(Tos:Overtemperature shutdown).
 * @return 返回温度值，单位是华氏度.
 * @n 温度范围是 -67°F 到 +257°F.
 */
float getTosF(void );

/**
 * @brief 获取滞后限制温度(自定义的温度点，小等于于阈值温度)..
 * @return 返回温度值，单位是摄氏度.
 * @n 温度值范围是 -55°C 到 +125°C.
 */
float getHysteresisC();

/**
 * @brief 获取滞后限制温度(自定义的温度点，小等于于阈值温度)..
 * @return 返回温度值，单位是华氏度.
 * @n 温度范围是 -67°F 到 +257°F.
 */
float getHysteresisF();

/**
 * @brief 设置阈值温度(自定义的温度点)
 * @param Tos 温度值，单位是摄氏度，需满足Tos% 0.5 == 0 ；
 * @n 范围是 -55°C 到 +125°C
 */
void setTosC(float Tos);
/**
 * @brief 设置阈值温度(自定义的温度点)
 * @param Tos 温度值，单位是华氏度，需满足Tos% 0.5 == 0 ；
 * @n 温度范围是 -67°F 到 +257°F
 */
void setTosF(float TosF);
/**
 * @brief 自定义滞后限制温度
 * @param Thyst 温度值，单位是摄氏度(°C)，需满足Thyst%0.5 == 0 ；
 * @n 范围是 -55°C 到 +125°C,Thyst 必须小于等于 Tos 的值.
 * @n 用户设定的滞后温度，会让OS电平的跳变从环境温度小于阈值温度延迟到小于滞后限制温度时再跳变.
 * @n 滞后限制温度产生的效果：当温度大于阈值温度时，OS Pin 变为活跃状态(默认为低电平)，当温度小于阈
 * @n 值温度时，OS Pin状态不会立即恢复正常状态(默认为高电平)，而是会延迟到小于滞后温度时才会恢复正常状态 
 */
void setHysteresisC(float Thyst);

/**
 * @brief 自定义滞后限制温度
 * @param Thyst 温度值，单位是华氏度(°F)，需满足Thyst%0.5 == 0 ；
 * @n 温度范围是 -67°F 到 +257°F,Thyst 必须小于等于 Tos 的值.
 */
void setHysteresisF(float ThystF);

/**
 * @brief 获取故障队列的值.
 * @return 返回故障队列的值.
 */
eQueueValue_t getQueueValue();

/**
 * @brief 设置故障队列的值.
 * @param value eQueueValue_t类型的值，代表故障队列数
 * @n value的取值为：
 * @n 温度寄存器存储的温度值在每次转换完成之后，会自动与阈值温度和滞后温度相比较。
 * @n eValue1，需满足一次温度值大于阈值温度。若满足则OS输出为active状态；
 * @n eValue2，需满足连续二次温度值大于阈值温度。若满足则OS输出为active状态。
 * @n eValue3，需满足连续四次次温度值大于阈值温度。若满足则OS输出为active状态。
 * @n eValue4，需满足连续六次温度值大于阈值温度。若满足则OS输出为active状态。
 */
void setQueueValue(eQueueValue_t value);

/**
 * @brief 得到芯片的工作模式的模式.
 * @return 0或1 .
 * @n 0：Normal(正常模式)
 * @n 1：interrupt(关断模式)
 */
eShutDownMode_t getShutDownMode();

/**
 * @brief 设置芯片的工作模式.
 * @param ShutDownMode eQueueValue_t类型的值，代表芯片工作模式
 */
void setShutDownMode(eShutDownMode_t ShutDownMode);

/**
 * @brief 得到OS引脚的active状态是高电平还是低电平.
 * @return 0或1 .
 * @n 0：LOW(active LOW)
 * @n 1：HIGH(active HIGH)
 */
eOSPolarityMode_t getOSPolarityMode();
/**
 * @brief 设置OS引脚的active状态是高电平还是低电平..
 * @param polarityMode eOSPolarityMode_t类型的值，代表OS引脚的极性
 */
void setOSPolarityMode(eOSPolarityMode_t polarityMode);

/**
 * @brief 得到OS引脚的工作模式.
 * @return 0或1 .
 * @n 0：OS comparator(比较器模式)
 * @n 1：OS interrupt(中断模式)
 */
eOSMode_t getOSMode();

/**
 * @brief 设置OS引脚的工作模式..
 * @param OSMode eOSMode_t类型的值，代表OS引脚的工作模式.
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

- data 2019-7-31
- version V0.1


## Credits

Written by fengli(li.feng@dfrobot.com), 2019.7.31 (Welcome to our [website](https://www.dfrobot.com/))





