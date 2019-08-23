/*!
 * @file highTempAlarm.ino
 * @brief High temperature alarm
 * @n Experiment phenomenon: set the threshold temperature Tos and hysteresis temperature Thyst(should be equal to Tos) first.
 * @n Set chip working state, OS pin output mode, and fault queue. 
 * @n The serial port displays a high temperature alarm when the temperature exceeds threshold temperature Tos, or to add a buzzer
 * @n on Arduino to warn High temperature.
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
  // Set the hysteresis and the threshold temperature to the same value, when ambient temperature is more than or less than
  // threshold value, OS pin will show different status, by which the high temperature detection can be realized.
  lm75b.setHysteresisC(/*Thyst=*/33);
  //Set hysteresis register to Fahrenheit 
  //lm75b.setHysteresisF(/*Thyst=*/91);
  
  
  /*!
    Set chip working mode 
    ShutDownMode Value: 
    eNormal  In this mode, data acquisition cycle is 100ms, among which 10ms is used for data conversion and it needs 300uA current.
            The rest is in idle state and it needs 10uA.
    eShutdown In this mode, data acquisition stops, but IIC communication and register works normally.
  */
  lm75b.setShutDownMode(/*ShutDownMode=*/lm75b.eNormal);
  
  /*!
    The OS output active state can be selected as HIGH or LOW by programming bit B2
    (OS_POL) of register Conf
     polarityMode Value: 
     eActive_LOW   OS active state is LOW in this mode 
     eActive_HIGH   OS active state is HIGH in this mode 
     When temperature is more than threshold temperature, OS output is active state (defalut: LOW).
  */
  lm75b.setOSPolarityMode(/*polarityMode=*/lm75b.eActive_LOW);
  
  /*!
    Set OS pin mode 
    OSMode Value:
    eComparator   In comparator mode, OS becomes active when the Temp exceeds the Tth(ots), and is reset 
                       when the Temp drops below the Thysh
    eInterrupt    In interrupt mode, once the OS output has been activated by crossing Tth(ots) 
                       and then reset, it can be activated again only when the Temp drops below the Thys
  */
  lm75b.setOSMode(/*OSMode=*/lm75b.eComparator);
  
  /*!
    OS generates interrupt only when there is enough number of queue faults. 
    value: 
    Each time the temperature in the temperature register completes conversion, it will automatically
    be compared with threshold and hystersis temperature. 
    eValue1, if temperature is more than threshold value once, OS output active state; 
    eValue2, if two successive temperatures are more than threshold value, OS output active state; 
    eValue3, if four successive temperatures are more than threshold value, OS output active state; 
    eValue4, if six successive temperatures are more than threshold value, OS output active state.
  */
  lm75b.setQueueValue(/*value=*/lm75b.eValue4);
  
  //User-defined value, OS state changes when ambient temperature exceeds this value.
  /*getTosC Get the threshold value stored in Tos register. 
  */
  Serial.println("**-----------------------------------------------------**");
  Serial.print("Threshold (degree Celsius): ");
  Serial.print(lm75b.getTosC());
  Serial.println("°C");
  /**
   * @brief Get threshold temperature(Tos:Overtemperature shutdown).
   * @return Return temperature value, unit: °F 
   * @n Detection range: -67°F to +257°F.
   */
  //Serial.print("Threshold (Fahrenheit): ");
  //Serial.print(lm75b.getTosF());
  //Serial.println("°F");
  Serial.println("**-----------------------------------------------------**");
}

void loop(void) {
  //Detect the OS state to check if the temperaure exceeds the pre-set value. 
   /*!
    Default setiing device operation mode selection：(0*)normal
            OS operation mode selection    ：(0*)OS comparator
            OS polarity selection          ：(0*)OS active LOW
            OS fault queue programming     ：(00*)queue value = 1
            reserved                       ： 000*
  */
  //Since polarity is set to active LOW mode, OS output LOW when temperature is over threshold. 
  while (digitalRead(OSPin) == 0) {
    Serial.println("Warning: Ambient temperature exceeds threshold vaule");
    delay(3000);
  }
  Serial.print("Ambient temperature: ");
  /*getTempC Get ambient temperature */
  Serial.print(/*Temperature=*/lm75b.getTemperatureC());
  Serial.println("°C");
  delay(3000);
}
