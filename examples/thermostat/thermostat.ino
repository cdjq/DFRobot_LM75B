/*!
 * @file thermostat.ino
 * @brief 恒温器，让目标设备保持在阈值温度处,温度变化的下限不能低于滞后限制温度.
 * @n 实验现象：在开始之前我们会设置阈值温度Tos(自定义的温度点)和滞后限制温度Thyst(自定义的温度点，小等于于阈值温度)
 * @n 而让温度保持在这个范围内。
 * @n 我们模拟外部环境，当串口打印超过阈值温度Tos时降温，在温度低于滞后限制温度Thyst时升温，达到
 * @n 一个恒温装置的效果
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
//OS引脚连接Arduino数字引脚2，通过引脚2监控OS脚的电平变化
#define OSPin   (2)
//state是OS 引脚的电平 1为正常状态，0为活跃状态
volatile  int state = 1;
//false 表示还没有进行恒温调节
//true  表示已经开始进行恒温调节
volatile bool thermostatState = false;
/*!
  thermostat 中断函数，功能是提示恒温器升温或降温.
*/
void thermostat(){
  //因为 polarity 选择的是active LOW模式，所以当温度值大于阈值温度，OS输出为低电平
  //将恒温点定在阈值温度处，温度变化的下限不能低于滞后限制温度  
  state = 1 - state;
  //开始进行恒温调节
  thermostatState = true;
}
void setup(void) {
  Serial.begin(115200);
  //检测IIC是否能正常通信
  while (lm75b.begin() != 0) {
    Serial.println("IIC初始化失败，请检测连线是否正确");
    delay(1000);
  }
  attachInterrupt(/*中断号0代表2号数字脚*/0,thermostat, CHANGE);  //设置触发类型为CHANGE，中断号0，即数字2口
  pinMode(OSPin, INPUT);
  /**
   @brief 自定义阈值温度(Tos:Overtemperature shutdown)
   @param 温度值，单位是摄氏度，需满足Tos%0.5 == 0 ；
   @n 范围是 -55°C 到 +125°C
  */
  lm75b.setTosC(/*Tos=*/33);
  //使用华氏度对阈值寄存器设置    
  //lm75b.setTosF(/*Tos=*/91);
  /**
   @brief 自定义滞后限制温度
   @param 温度值，单位是摄氏度，需满足Thyst%0.5 == 0 ；
   @n 范围是 -55°C 到 +125°C,Thyst 必须小于等于 Tos 的值.
   @n 因为滞后限制值是阈值温度的下偏差的限制值，所以设定的温度应应尽量接近阈值温度，以达到比较精确地控制温度.
   @n 用户设定的滞后温度，会让OS电平的跳变从环境温度小于阈值温度时跳变滞后到小于滞后限制温度时再跳变.
   @n 滞后限制温度产生的效果：当温度大于阈值温度时，OS Pin 变为活跃状态(默认为低电平)，当温度小于阈
   @n 值温度时，OS Pin状态不会立即恢复正常状态(默认为高电平)，而是会延迟到小于滞后温度时才会恢复正常状态 
  */
  lm75b.setHysteresisC(/*Thyst=*/32);
  //使用华氏度对滞后寄存器设置
  //lm75b.setHysteresisF(/*Thyst=*/91);
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
  
  Serial.println("**-----------------------------------------------------**");
  //用户设定值，环境温度超出此值时引起OS状态变为active
  /*getTosC函数的作用时获取Tos寄存器里面存储的阈值大小，
  */
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
  /*!
    getHysteresisC函数的作用时获取Thyst寄存器里面存储的滞后限制温度的大小，
  */
  Serial.print("滞后温度(摄氏度): ");
  Serial.print(lm75b.getHysteresisC());
  Serial.println("°C");
  /**
   * @brief 获取滞后限制温度(自定义的温度点，小等于于阈值温度)..
   * @return 返回温度值，单位是华氏度.
   * @n 温度范围是 -67°F 到 +257°F.
   */
  //Serial.print("阈值温度(华氏度): ");
  //Serial.print(lm75b.getTosF());
  //Serial.println("°F");
  Serial.println("**-----------------------------------------------------**");
    if(lm75b.getTemperatureC()<lm75b.getHysteresisC()) {
      Serial.println("环境温度低于滞后温度，请升温");
    }
    else if(lm75b.getTemperatureC()>lm75b.getTosC()){
      Serial.println("环境温度高于阈值温度，请降温");
    }
    else{
      Serial.println("温度正常");
    }
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
  //将恒温点定在阈值温度处，温度变化的下限不能低于滞后限制温度
  delay(2000);
  Serial.print("环境温度: ");
  /*getTempC 获取环境温度*/
  Serial.print(/*温度=*/lm75b.getTemperatureC());
  Serial.println("°C");
  if((state == 0) && (thermostatState == true)){
   Serial.println("降温阶段，将温度降至滞后温度以下");
  } 
  else if ((state == 1) && (thermostatState == true)) {
   Serial.println("升温阶段，将温度升至阈值温度以上");
  }
}