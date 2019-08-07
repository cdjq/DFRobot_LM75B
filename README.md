# DFRobot_LM75B
LM75B 是一个使用了内置带隙温度传感器和Σ-△模数转换技术的温度-数字转换器,能监测环境温度,当温度超过自定义的阈值时,<br>
它可提供一个过热检测输出(OS pin),并且可以防止噪音干扰<br>
LM75B器件通过两线的串行IIC 总线接口与控制器通信,LM75B 有3个可选的逻辑地址管脚，使得同一总线上可同时连接8个器件而不发生地址冲突<br>
LM75B 可配置成不同的工作模式。它可设置成在正常工作模式下周期性地对环境温度进行监控或进入关断模式来将器件功耗降至最低.<br>
  分辨率：0.125°C <br>
  精度  ：正负 2 °C <br>
  功耗  :工作电流典型值为200uA，掉电模式为1.0uA<br>
  工作电压：2.8V～5.5V<br>
![正反面svg效果图](https://github.com/ouki-wang/DFRobot_Sensor/raw/master/resources/images/SEN0245svg1.png)

## 产品链接（链接到英文商城）
    SKU：LM75B 温度传感器
   
## Table of Contents

* [Summary](#summary)
* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary

   1.读取环境温度,会分别打印摄氏度和华氏度两种单位的温度值<br>
   2.在串口打印芯片的设置(芯片工作模式,OS极性,OS输出模式,故障队列数,阈值温度,滞后限制温度)<br>
   3.用户定义温度阈值，当温度超过阈值温度时，串口会有高温提示<br>
   4.作为恒温器的监视装备，让设备的温度维持在自定义的阈值温度处<br>

## Installation

To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

## Methods

```C++
    
/**
 * @brief 构造函数
 * @param pWire I2C总线指针对象，构造设备，可传参数也可不传参数，默认Wire
 * @param addr 7位I2C地址,由前三位决定地址的值，取值(0x48/0x49/0x4A/0x4B/0x4C/0x4D/0x4E/0x4F)默认0x48
 * @n IIC地址由构成如下图所示
        6  5  4  3  2  1   0
        1  0  0  1  A2 A1  A0
 * @n 地址的定义如下表所示,可以通过跳线来改变地址：默认为0x48
        1  0  0  1  | A2 A1 A0
        1  0  0  1  | 1  1  1       0x4F
        1  0  0  1  | 1  1  0       0x4E
        1  0  0  1  | 1  0  1       0x4D
        1  0  0  1  | 1  0  0       0x4C
        1  0  0  1  | 0  1  1       0x4B
        1  0  0  1  | 0  1  0       0x4A
        1  0  0  1  | 0  0  1       0x49
        1  0  0  1  | 0  0  0       0x48
 */
DFRobot_LM75B(TwoWire *pWire = &Wire, uint8_t address = 0x48) ;

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
 * @brief 获取阈值温度(Tos:Overtemperature shutdown)..
 * @return 返回温度值，单位是摄氏度.
 * @n 温度值范围是 -55°C 到 +125°C.
 */
float getTosC(void );

/**
 * @brief 获取滞后限制温度(自定义的温度点，小等于于阈值温度)..       
 * @return 返回温度值，单位是摄氏度.
 * @n 温度值范围是 -55°C 到 +125°C.
 */
float getHysteresisC();

/**
 * @brief 设置阈值温度
 * @param Tos 温度值，单位是摄氏度，需满足Tos% 0.5 == 0 ；
 * @n 可设置温度值范围是 -55°C 到 +125°C
 */
void setTos(float Tos);

/**
 * @brief 自定义滞后限制温度
 * @param 温度值，单位是摄氏度，需满足Thyst%0.5 == 0 ；
 * @n 范围是 -55°C 到 +125°C,Thyst 必须小于等于 Tos 的值.
 * @n 用户设定的滞后温度，会让OS引脚电平的跳变点从环境温度小于阈值温度时跳变滞后到小于滞后限制温度时再跳变.
 * @n 滞后限制温度产生的效果：当温度大于阈值温度时，OS Pin 变为活跃状态(默认为低电平)，当温度小于阈
 * @n 值温度时，OS Pin状态不会立即恢复正常状态(默认为高电平)，而是会延迟到小于滞后温度时才会恢复正常状态 
 */
void setHysteresis(float Thyst);

/**
 * @brief 获取故障队列的值.
 * @return 返回故障队列的值.
 */
eQueueValue_t getQueueValue();

/**
 * @brief 设置故障队列的值，故障队列定义为必须连续发生才能激活OS输出的的故障数量
 * @param value eQueueValue_t类型的值，代表故障队列数
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





