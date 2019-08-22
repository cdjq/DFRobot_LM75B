/*!
 * @file highTempAlarm.ino
 * @brief High temperature alarm
 * @n Experiment phenomenon: set the threshold temperature Tos and hysteresis temperature Thyst(should be equal to Tos) first.
 * @n Set chip working state, OS pin output mode, fault queue, when the 当温度超过阈值温度Tos时串口就会有信息提示，或者也可
 * @n 以在arduino上加一个蜂鸣器来提醒温度超过阈值
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
//如果需要自己定义软IIC和改变芯片地址，便使用此构造函数
//DFRobot_LM75B lm75b(&Wire, 0x48);
DFRobot_LM75B lm75b; 
//OS引脚连接Arduino数字引脚2，通过引脚4监控OS脚的电平变化
#define OSPin   (2)

void setup(void) {
  Serial.begin(115200);
  //检测IIC是否能正常通信.
  while (lm75b.begin() != 0) {
    Serial.println("IIC初始化失败，请检测连线是否正确");
    delay(1000);
  }
  pinMode(OSPin, INPUT);
  
  /**
    @brief 设置阈值温度(Tos:Overtemperature shutdown)
    @param 温度值，单位是摄氏度，需满足Tos%0.5 == 0 ；
    @n 范围是 -55°C 到 +125°C
  */
  lm75b.setTosC(/*Tos=*/33);
  //使用华氏度对阈值寄存器设置    
  //lm75b.setTosF(/*Tos=*/91);
  /**
    @brief setHysteresisC自定义滞后温度的大小
    @param 温度值，单位是摄氏度，需满足Thyst%0.5 == 0 ；
    @n 范围是 -55°C 到 +125°C,Thyst 必须小于等于 Tos 的值.
    @n 用户设定的滞后温度，会让OS电平的跳变从环境温度小于阈值温度时跳变滞后到小于滞后限制温度时跳变.
    @n 滞后限制温度产生的效果：当温度大于阈值温度时，OS Pin 变为活跃状态(默认为低电平)，当温度小于阈
    @n 值温度时，OS Pin状态不会立即恢复正常状态(默认为高电平)，而是会延迟到小于滞后温度时才会恢复正常状态 
  */
  //将滞后温度和阈值温度设置相同，那么就在超过阈值温度时OS的状态和低于阈值温度时的状态不一样，就可以做到超温检测.
  lm75b.setHysteresisC(/*Thyst=*/33);
  //使用华氏度对滞后寄存器设置
  //lm75b.setHysteresisF(/*Thyst=*/91);
  
  
  /*!
    设置芯片工作模式
    ShutDownMode的取值为：
    eNormal  在此模式下，数据采集周期为100ms,其中10ms用于数据转换，需要电流为300uA，另外90ms处于idle状态，需要电流为10uA
    eShutdown 在此模式下，数据采集停止，但IIC通信不受影响，寄存器也可以正常读写
  */
  lm75b.setShutDownMode(/*ShutDownMode=*/lm75b.eNormal);
  
  /*!
    The OS output active state can be selected as HIGH or LOW by programming bit B2
    (OS_POL) of register Conf
     polarityMode的取值为：
     eActive_LOW   在此模式下，OS的active状态为低电平
     eActive_HIGH   在此模式下，OS的active状态为高电平
    当温度值大于阈值温度，若满足则OS输出为active状态，active状态默认为低电平。
  */
  lm75b.setOSPolarityMode(/*polarityMode=*/lm75b.eActive_LOW);
  
  /*!
    设置设置OS引脚的模式
    OSMode的取值为：
    eComparator   OS口输出采用比较器模式，OS becomes active when the Temp exceeds the Tth(ots), and is reset 
                       when the Temp drops below the Thysh
    eInterrupt    在此模式下，OS口输出采用中断模式,Once the OS output has been activated by crossing Tth(ots) 
                       and then reset, it can be activated again only when the Temp drops below the Thys
  */
  lm75b.setOSMode(/*OSMode=*/lm75b.eComparator);
  
  /*!
    只有满足故障队列数，OS才会产生中断
    value的取值为：
    温度寄存器存储的温度值在每次转换完成之后，会自动与阈值温度和滞后温度相比较。
    eValue1，需满足一次温度值大于阈值温度。若满足则OS输出为active状态；
    eValue2，需满足连续二次温度值大于阈值温度。若满足则OS输出为active状态。
    eValue3，需满足连续四次次温度值大于阈值温度。若满足则OS输出为active状态。
    eValue4，需满足连续六次温度值大于阈值温度。若满足则OS输出为active状态。
  */
  lm75b.setQueueValue(/*value=*/lm75b.eValue4);
  
  //用户设定值，环境温度超出此值时引起OS状态改变
  /*getTosC函数的作用时获取Tos寄存器里面存储的阈值大小，
  */
  Serial.println("**-----------------------------------------------------**");
  Serial.print("阈值温度(摄氏度): ");
  Serial.print(lm75b.getTosC());
  Serial.println("°C");
  /**
   * @brief 获取阈值温度(Tos:Overtemperature shutdown).
   * @return 返回温度值，单位是华氏度.
   * @n 温度范围是 -67°F 到 +257°F.
   */
  //Serial.print("阈值温度(华氏度): ");
  //Serial.print(lm75b.getTosF());
  //Serial.println("°F");
  Serial.println("**-----------------------------------------------------**");
}

void loop(void) {
  //检测OS的状态来判断温度是否超过设定值
   /*!
    默认设置 device operation mode selection：(0*)normal
            OS operation mode selection    ：(0*)OS comparator
            OS polarity selection          ：(0*)OS active LOW
            OS fault queue programming     ：(00*)queue value = 1
            reserved                       ： 000*
  */
  //因为 polarity 选择的是active LOW模式，所以当温度值大于阈值温度，OS输出为低电平
  while (digitalRead(OSPin) == 0) {
    Serial.println("环境温度超过阈值温度，请注意");
    delay(3000);
  }
  Serial.print("环境温度: ");
  /*getTempC 获取环境温度*/
  Serial.print(/*温度=*/lm75b.getTemperatureC());
  Serial.println("°C");
  delay(3000);
}
