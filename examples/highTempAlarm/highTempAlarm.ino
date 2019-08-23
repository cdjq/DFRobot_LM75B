/*!
 * @file highTempAlarm.ino
 * @brief High temperature alarm
 * @n Experiment phenomenon: set the threshold temperature Tos and hysteresis temperature Thyst(should be equal to Tos) first.
 * @n Set chip working state, OS pin output mode, and fault queue. 
 * @n The serial port displays a high temperature alarm when the temperature exceeds threshold temperature Tos, or to add a buzzer on Arduino to warn High temperature.
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
 * @n Value(0x48/0x49/0x4A/0x4B/0x4C/0x4D/0x4E/0x4F) default 0x48
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
//Connect OS pin to Arduino digital pin 2, monitor the level of OS pin via pin 4. 
#define OSPin   (2)

void setup(void) {
  Serial.begin(115200);
  //Detect if IIC communication works properly
  while (lm75b.begin() != 0) {
    Serial.println("IIC init failed, please check if the connection is correct?");
    delay(1000);
  }
  pinMode(OSPin, INPUT);
  
  /**
    @brief Set threshold temperaure (Tos:Overtemperature shutdown)
    @param Temperature value, unit: °C, Tos%0.5 == 0;
    @n Detection range: -55°C to +125°C
  */
  lm75b.setTosC(/*Tos=*/33);
  //Set threshold register to Fahrenheit     
  //lm75b.setTosF(/*Tos=*/91);
  /**
    @brief setHysteresisC Set hysteresis temperature 
    @param Temperature, unit: °C, Thyst%0.5 == 0;
    @n Detection range: -55°C to +125°C, Thyst must be less than Tos 
    @n User-defined hysteresis temperature; delay the level jump of OS: OS level will jump when the
    @n ambient temperature is less than hysteresis value instead of threshold value.
    @n Effect: when the temperature is more than threshold temperature, OS pin becomes active(default LOW)
    @n         When the temperature is less than threshold temperature, OS pin will not back to the normal
    @n state(default HIGH) until the temperature value is less than the hysteresis tempreature. 
  */
  // Set the hysteresis and the threshold temperature to the same value, when ambient temperature is more than or less than threshold value, OS pin will show different status, by which the high temperature detection can be realized.
  lm75b.setHysteresisC(/*Thyst=*/33);
  //Set hysteresis register to Fahrenheit 
  //lm75b.setHysteresisF(/*Thyst=*/91);
  
  
  /*!
    Set chip working mode 
    ShutDownMode Value: 
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
