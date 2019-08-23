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
    eValue1=1, if temperature is more than threshold value once, OS output active state;
    eValue2=2, if two successive temperatures are more than threshold value, OS output active state; 
    eValue3=4, if four successive temperatures are more than threshold value, OS output active state; 
    eValue4=6, if six successive temperatures are more than threshold value, OS output active state.
   */
  Serial.print("OS故障队列: ");
  Serial.print(lm75b.getQueueValue());
  Serial.println(" (OS输出变为active状态需要连续满足环境温度大于阈值温度的次数)");
  //用户设定值，环境温度超出此值时引起OS状态改变
  /*getTosC函数的作用时获取Tos寄存器里面存储的阈值大小，
  */
  Serial.print("阈值温度(摄氏度): ");
  /**
   * @brief 获取阈值温度(Tos:Overtemperature shutdown).
   * @return 返回温度值，单位是摄氏度.
   * @n 温度值范围是 -55°C 到 +125°C.
   */
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
  /*!
    getHysteresisC函数的作用时获取Thyst寄存器里面存储的滞后限制温度的大小，
    用户设定的滞后温度，会让OS电平的跳变从环境温度小于阈值温度时跳变滞后到小于滞后限制温度时跳变.
    滞后限制温度产生的效果：当温度大于阈值温度时，OS Pin 变为活跃状态(默认为低电平)，当温度小于阈
    值温度时，OS Pin状态不会立即恢复正常状态(默认为高电平)，而是会延迟到小于滞后温度时才会恢复正常状态 
  */
  Serial.print("滞后温度(华氏度): ");
  Serial.print(lm75b.getHysteresisC());
  Serial.println("°C");
  /**
   * @brief 获取滞后限制温度(自定义的温度点，小等于于阈值温度)..
   * @return 返回温度值，单位是华氏度.
   * @n 温度范围是 -67°F 到 +257°F.
   */
  //Serial.print("滞后温度(华氏度): ");
  //Serial.print(lm75b.getHysteresisF());
  //Serial.println("°F");
  Serial.println("**-----------------------------------------------------**");
}

void loop(void) {
  Serial.print("环境温度(摄氏度): ");
  Serial.print(/*摄氏温度=*/lm75b.getTemperatureC());
  Serial.println("°C");
  Serial.print("环境温度(华氏度): ");
  Serial.print(/*华氏温度=*/lm75b.getTemperatureF());
  Serial.println("°F");
  Serial.println("");
  // 设定的延时应大于100ms；
  delay(1000);
}
