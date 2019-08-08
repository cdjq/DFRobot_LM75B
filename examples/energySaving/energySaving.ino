/*!
 * @file energySaving.ino
 * @brief 当读取到温度值后芯片进入节能模式，20秒后恢复正常模式继续读取温度，
 * @n 然后继续进入节能模式，一直循环.
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

void setup(void) {
  Serial.begin(115200);
  //检测IIC是否能正常通信
  while (lm75b.begin() != 0) {
    Serial.println("IIC初始化失败，请检测连线是否正确");
    delay(1000);
  }
}

void loop(void) {
  /*!
   设置芯片工作模式
   ShutDownMode的取值为：
   eNormal  在此模式下，数据采集周期为100ms,其中10ms用于数据转换，需要电流为300uA，另外90ms处于idle状态，需要电流为10uA
   eShutdown 在此模式下，数据采集停止，但IIC通信不受影响，寄存器也可以正常读写
  */
  lm75b.setShutDownMode(/*ShutDownMode=*/lm75b.eNormal);
  //需要一定的数据转换周期
  delay(200);
  Serial.println("芯片进入正常模式,读取温度........");
  Serial.print("环境温度(摄氏度): ");
  Serial.print(/*摄氏温度=*/lm75b.getTemperatureC());
  Serial.println("°C");
  Serial.print("环境温度(华氏度): ");
  Serial.print(/*华氏温度=*/lm75b.getTemperatureF());
  Serial.println("°F");
  Serial.println("芯片进入节能模式........");
  lm75b.setShutDownMode(/*ShutDownMode=*/lm75b.eShutdown);
  // 每二十秒读取一次温度然后立刻进入节能模式.
  delay(20000);
}