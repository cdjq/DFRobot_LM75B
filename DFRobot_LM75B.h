/*!
 * @file DFRobot_LM75B.h
 * @brief 定义DFRobot_LM75B 类的基础结构
 * @n 这是一个数字温度传感器的库，用来读取环境温度
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-07-28
 * @get from https://www.dfrobot.com
 * @https://github.com/DFRobot/DFRobot_LM75B
*/
#ifndef DFROBOT_LM75B_H
#define DFROBOT_LM75B_H
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>

#define REG_LM75B_TEMP    0x00
#define REG_LM75B_CONF    0x01
#define REG_LM75B_THYST   0x02
#define REG_LM75B_TOS     0x03

//#define ENABLE_DBG

#ifdef ENABLE_DBG
#define DBG(...) {Serial.print("[");Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif


class DFRobot_LM75B {

public:

#define ERR_OK             0      //无错误
#define ERR_DATA_BUS      -1      //数据总线错误
#define ERR_IC_VERSION    -2      //芯片版本不匹配

  /*!
       The Configuration register (Conf) is a write/read register and contains an 8-bit
   non-complement data byte that is used to configure the device for different operation
   conditions.
   ------------------------------------------------------------------------------------------
   |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
   ------------------------------------------------------------------------------------------
   |            reserved            |       OS_F_QUE      |   OS_POL |OS_COMP_INT|SHUTDOWN  |
   ------------------------------------------------------------------------------------------
   默认设置  device operation mode selection：(0*)normal
             OS operation mode selection    ：(0*)OS comparator
             OS polarity selection          ：(0*)OS active LOW
             OS fault queue programming     ：(00*)queue value = 1
             reserved                       ： 000*
  */
  typedef struct {
    uint8_t sSHUTDOWN: 1;
    uint8_t sOS_COMP_INT: 1;
    uint8_t sOS_POL: 1;
    uint8_t sOS_F_QUE: 2;
    uint8_t sRESERVED: 3;
  } __attribute__ ((packed)) sMode_t;
  /*!
        The LM75B can be configured for different operation conditions. It can be set in normal
   mode to periodically monitor the ambient temperature, or in shutdown mode to minimize
   power consumption.The device operation mode is selected by programming bit B0 (SHUTDOWN) of 
   register Conf. 
   */
  typedef enum {
    eNormal = 0, /**<在此模式下，数据采集周期为100ms,其中10ms用于数据转换，需要电流为200uA，另外90ms处于idle状态，需要电流为10uA>**/
    eShutdown = 1 /**<在此模式下，数据采集停止，但IIC通信不受影响，寄存器也可以正常读写>**/
  } eShutDownMode_t;
  /*!
         The main difference between the two modes is that in OS comparator mode, the OS
    output becomes active when Temp has exceeded Tth(ots) and reset when Temp has
    dropped below Thys, reading a register or putting the device into shutdown mode does not
    change the state of the OS output; while in OS interrupt mode, once it has been activated
    either by exceeding Tth(ots) or dropping below Thys, the OS output will remain active
    indefinitely until reading a register, then the OS output is reset.The OS mode is selected 
    by programming bit B1 (OS_COMP_INT) of register Conf.
  */
  typedef enum {
    eComparator = 0x00, /**<在此模式下，OS口输出采用比较器模式，>**/
    eInterrupt = 0x01 /**<在此模式下，OS口输出采用中断模式>**/
  } eOSMode_t;
  /*!
         The OS output active state can be selected as HIGH or LOW by programming bit B2
    (OS_POL) of register Conf
  */
  typedef enum {
    eActive_LOW = 0,  /**<在此模式下，OS的active状态为低电平>**/
    eActive_HIGH = 1  /**<在此模式下，OS的active状态为高电平>**/
  } eOSPolarityMode_t;
  /*!
         It is provided to avoid false tripping due to noise.fault queue is also defined as the number of
    consecutive conversions returning a temperature trip.The value of fault queue is selectable by programming
    the two bits B4 and B3 (OS_F_QUE[1:0]) in register Conf.
  */
  typedef enum {
    eValue1 = 1,/**<在此模式下，故障队列数为 1>**/
    eValue2 = 2,/**<在此模式下，故障队列数为 2>**/
    eValue3 = 4,/**<在此模式下，故障队列数为 4>**/
    eValue4 = 6 /**<在此模式下，故障队列数为 6>**/
  } eQueueValue_t;
  /*!
   * @brief 构造函数
   * @param pWire I2C总线指针对象，构造设备，可传参数也可不传参数，默认Wire
   * @param addr 7位I2C地址,由前三位决定地址的值，取值(0x48/0x49/0x4A/0x4B/0x4C/0x4D/0x4E/0x4F)默认0x48
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
   * @brief 获取滞后限制温度(自定义的温度点，小等于于阈值温度)..
   * @return 返回温度值，单位是摄氏度.
   * @n 温度值范围是 -55°C 到 +125°C.
   */
  float getHysteresisC();
  
  /**
   * @brief 设置阈值温度(自定义的温度点)
   * @param Tos 温度值，单位是摄氏度，需满足Tos% 0.5 == 0 ；
   * @n 范围是 -55°C 到 +125°C
   */
  void setTos(float Tos);
  
  /**
   * @brief 设置滞后限制温度(自定义的温度点，小等于于阈值温度).
   * @param Thyst 温度值，单位是摄氏度，需满足Thyst% 0.5 == 0 ；
   * @n 范围是 -55°C 到 +125°C,Thyst 必须小于等于 Tos 的值.
   */
  void setHysteresis(float Thyst);

  /**
   * @brief 获取故障队列的值.
   * @return 返回故障队列的值.
   */
  eQueueValue_t getQueueValue();
  
  /**
   * @brief 设置故障队列的值.
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

private:
  
  /**
   * @brief 通过IIC总线写入寄存器值
   * @param reg  寄存器地址 8bits
   * @param pBuf 要写入数据的存放缓存
   * @param size 要写入数据的长度
   * @return 返回实际读取的长度，返回0表示读取失败
   */
  void writeReg(uint8_t reg, void* pBuf, size_t size) ;

  /**
   * @brief 读取寄存器函数，设计为虚函数，由派生类实现函数体
   * @param reg  寄存器地址 8bits
   * @param pBuf 要写入数据的存放缓存
   * @param size 要写入数据的长度
   * @return 返回实际读取的长度，返回0表示读取失败
   */
  uint8_t readReg(uint8_t reg, void* pBuf, size_t size);
  TwoWire *_pWire;
  uint8_t _address;
};

#endif
