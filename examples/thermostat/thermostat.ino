/*!
 * @file thermostat.ino
 * @brief 恒温器，让设备保持在一定的温度范围.
 * @n 实验现象：在开始之前我们会设置阈值温度Tos和滞后温度Thyst，而让温度保持在这个范围内
 * @n 我们模拟外部环境，当串口打印超过阈值温度Tos时降温，在温度低于滞后温度Thyst时升温，达到
 * @n 一个恒温装置的效果
 *
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-07-30
 * @get from https://www.dfrobot.com
 * @https://github.com/DFRobot/DFRobot_LM75B
 */
#include <DFRobot_LM75B.h>
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
DFRobot_LM75B lm75b(&Wire, 0x48);
#define OS   (4)
void setup(void) {
  Serial.begin(115200);
  //检测IIC是否能正常通信
  while (lm75b.begin() != 0) {
    Serial.println("IIC初始化失败，请检测连线是否正确");
    delay(1000);
  }
  pinMode(OS, INPUT);
  /**
   @brief 设置阈值温度
   @param 温度值，单位是摄氏度，需满足Tos%0.5 == 0 ；
   @n 范围是 -55°C 到 +125°C
  */
  lm75b.setTos(/*Tos=*/33);
  /**
       @brief 设置滞后温度
       @param 温度值，单位是摄氏度，需满足Thyst%0.5 == 0 ；
       @n 范围是 -55°C 到 +125°C,Thyst 必须小于等于 Tos 的值.
  */
  lm75b.setThyst(/*Thyst=*/30.5);
  /*!
    设置故障队列数,只有满足故障队列数，OS才会产生中断
    故障队列数：温度寄存器存储的温度值在每次转换完成之后，会自动与阈值温度和滞后温度相比较。
    当选择eValue1，只需满足一次温度值大于阈值温度。若满足则OS输出为active(见下)状态；
    当选择eValue2，需满足二次温度值大于阈值温度。若满足则OS输出为active状态。
    以此类推。
    eValue的取值为：
    typedef enum {
    eValue1 = 1,
    eValue2 = 2,
    eValue3 = 4,
    eValue4 = 6
    } eQueueValue_t;
  */
  lm75b.setQueueValue(/*eValue=*/lm75b.eValue4);
}

void loop(void) {
  /*!
         默认设置 device operation mode selection：(0*)normal
                  OS operation mode selection    ：(0*)OS comparator
                  OS polarity selection          ：(0*)OS active LOW
                  OS fault queue programming     ：(00*)queue value = 1
                  reserved                       ： 000*
  */
  //因为 polarity 选择的是active LOW模式，所以当温度值大于阈值温度，OS输出为低电平
  if (digitalRead(OS) == 0) {
    Serial.println("环境温度超过阈值温度，请降温");
  }
  if (digitalRead(OS) == 1) {
    Serial.println("环境温度低于滞后温度，请升温");
  }
  delay(2000);

}