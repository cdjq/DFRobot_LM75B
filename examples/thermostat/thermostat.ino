/*!
 * @file thermostat.ino
 * @brief Thermostat, keep targeted device's temperature at threshold point. The minimum temperature should be greater than hysteresis value. 
 * @n Experiment phenomenon: set threshold temperature Tos and hysteresis temeperaure Thyst(≤ threshold), and keep the temperature within the range.
 * @n Simulate external environment: when serial print temperature exceeds Tos, lower the temperature; when less than Thyst, raise it. 
 * @n
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
//Connect OS pin to Arduino digital pin 2, monitor the level of OS pin via pin 2. 
#define OSPin   (2)
//State: the LEVEL of OS pin, 1 normal, 0 active
volatile  int state = 1;
//false thermostatic regulation not start 
//true  thermostatic regulation starts
volatile bool thermostatState = false;
/*!
  thermostat Interrupt function, prompt thermostat to increase or decrease temperature.
*/
void thermostat(){
  //Since polarity is set to active LOW mode, OS output LOW when temperature is over threshold
  //Define the constant temperature point at threshold point, the minimum changing temperature should be greater than hysteresis value.
  state = 1 - state;
  //Start thermostatic regulation 
  thermostatState = true;
}
void setup(void) {
  Serial.begin(115200);
  //Detect of IIC communication works properly
  while (lm75b.begin() != 0) {
    Serial.println("IIC init failed, please check if the connection is correct?");
    delay(1000);
  }
  attachInterrupt(/*Interrupt number 0 represents digital pin2*/0,thermostat, CHANGE);  //Set trigger type to CHANGE, interrupt number 0(D Pin2)
  pinMode(OSPin, INPUT);
  /**
   @brief Define threshold temperature(Tos:Overtemperature shutdown)
   @param Temperature, unit:°C, Tos%0.5 == 0 ；
   @n Detection range: -55°C  to +125°C
  */
  lm75b.setTosC(/*Tos=*/33);
  //Set threshold register to Fahrenheit    
  //lm75b.setTosF(/*Tos=*/91);
  /**
   @brief Defien hysteresis temperature 
   @param  Temperature, unit: °C, Thyst%0.5 == 0;
   @n Detection range: -55°C to +125°C, Thyst must be smaller than Tos;
   @n The pre-set temperature should approach threshold temperature so as to control temperature accurately. 
   @n User-defined hysteresis temperature; delay the level jump of OS: OS level will jump when the
   @n ambient temperature is less than hysteresis value instead of threshold value.
   @n Effect: when the temperature is more than threshold temperature, OS pin becomes active(default LOW)
   @n         When the temperature is less than threshold temperature, OS pin will not back to the normal
   @n state(default HIGH) until the temperature value is less than the hysteresis tempreature. 
  */
  lm75b.setHysteresisC(/*Thyst=*/32);
  //Set hysteresis register to Fahrenheit  
  //lm75b.setHysteresisF(/*Thyst=*/91);
  /*!
    OS generates interrupt only when there is enough number of queue faults. 
    value: 
    Each time the temperature in the temperature register completes conversion, it will automatically
    be compared with threshold and hystersis temperature. 
    eValue1, if one temperature vlaue is more than threshold value, OS output active state; 
    eValue2, if two successive temperature values are more than threshold value, OS output active state; 
    eValue3, if four successive temperature values are more than threshold value, OS output active state; 
    eValue4, if six successive temperature values are more than threshold value, OS output active state.
  */
  lm75b.setQueueValue(/*value=*/lm75b.eValue4);
  
  Serial.println("**-----------------------------------------------------**");
  //User-defined value, OS state changes to active when ambient temperature exceeds this value.
  /*getTosCGet the threshold value stored in Tos register.
  */
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
  /*!
    getHysteresisC Get hysteresis temperature stored in Thyst register
  */
  Serial.print("Hysteresis temperature (degree Celsius): ");
  Serial.print(lm75b.getHysteresisC());
  Serial.println("°C");
  /**
   * @brief Get hysteresis temperature (User-defined temperature, ≤ Threshold)..
   * @return Return temperature, unit: °F 
   * @n Detection range: -67°F to +257°F.
   */
  //Serial.print("Threshold temperature (Fahrenheit): ");
  //Serial.print(lm75b.getTosF());
  //Serial.println("°F");
  Serial.println("**-----------------------------------------------------**");
    if(lm75b.getTemperatureC()<lm75b.getHysteresisC()) {
      Serial.println("Ambient temperature less than Hysteresis, increase the temperature");
    }
    else if(lm75b.getTemperatureC()>lm75b.getTosC()){
      Serial.println("Ambient temperature more than Threshold, decrease the temperature");
    }
    else{
      Serial.println("Temperature is fine");
    }
}

void loop(void) {
  /*!
   Default setting: device operation mode selection：(0*)normal
            OS operation mode selection    ：(0*)OS comparator
            OS polarity selection          ：(0*)OS active LOW
            OS fault queue programming     ：(00*)queue value = 1
            reserved                       ： 000*
  */
  //Since polarity is set to active LOW mode, OS output LOW when temperature is over threshold
  //Define the constant temperature point at threshold point, the minimum changing temperature should be greater than hysteresis value.
  delay(2000);
  Serial.print("Ambient Temperature: ");
  /*getTempC Get ambient temperature*/
  Serial.print(/*Temperature=*/lm75b.getTemperatureC());
  Serial.println("°C");
  if((state == 0) && (thermostatState == true)){
   Serial.println("Temperature reducing stage: reduce the temperature under hysteresis temperature");
  } 
  else if ((state == 1) && (thermostatState == true)) {
   Serial.println("Temperature increasing stage: increase the temperature above threshold temperature");
  }
}
