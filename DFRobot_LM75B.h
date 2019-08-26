/*!
 * @file DFRobot_LM75B.h
 * @brief Define the basic structure of class DFRobot_LM75B.
 * @n This is a temperature sensor library for reading ambient temperature. 
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-07-28
 * @get from https://www.dfrobot.com
 * @url https://github.com/DFRobot/DFRobot_LM75B
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

#define ERR_OK             0      //OK
#define ERR_DATA_BUS      -1      //Data Bus error
#define ERR_IC_VERSION    -2      //Chip version mismatch 

  /*!
       The Configuration register (Conf) is a write/read register and contains an 8-bit
   non-complement data byte that is used to configure the device for different operation
   conditions.
   ------------------------------------------------------------------------------------------
   |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
   ------------------------------------------------------------------------------------------
   |            reserved            |       OS_F_QUE      |   OS_POL |OS_COMP_INT|SHUTDOWN  |
   ------------------------------------------------------------------------------------------
   Default setting  device operation mode selection：(0*)normal
             OS operation mode selection    ：(0*)OS comparator
             OS polarity selection          ：(0*)OS active LOW
             OS fault queue programming     ：(00*)queue value = 1
             reserved                       ： 000*
  */
  typedef struct {
    uint8_t SHUTDOWN: 1;
    uint8_t OS_COMP_INT: 1;
    uint8_t OS_POL: 1;
    uint8_t OS_F_QUE: 2;
    uint8_t RESERVED: 3;
  } __attribute__ ((packed)) sMode_t;
  /*!
        The LM75B can be configured for different operation conditions. It can be set in normal
   mode to periodically monitor the ambient temperature, or in shutdown mode to minimize
   power consumption.The device operation mode is selected by programming bit B0 (SHUTDOWN) of 
   register Conf. 
   */
  typedef enum {
    eNormal = 0, /**<In this mode, data acquisition cycle is 100ms, among which 10ms is used for data comversion and needs 200uA current, 
  the rest 90ms is in idle state and needs 10uA>**/
    eShutdown = 1 /**<In this mode, data acquisition, but IIC communication and register works normally.>**/
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
    eComparator = 0x00, /**<OS output adopts comparator mode>**/
    eInterrupt = 0x01 /**<OS output adopts interrupt mode>**/
  } eOSMode_t;
  /*!
         The OS output active state can be selected as HIGH or LOW by programming bit B2
    (OS_POL) of register Conf
  */
  typedef enum {
    eActive_LOW = 0,  /**<OS active state is LOW in this mode>**/
    eActive_HIGH = 1  /**<OS actie state is HIGH in this mode>**/
  } eOSPolarityMode_t;
  /*!
         It is provided to avoid false tripping due to noise.fault queue is also defined as the number of
    consecutive conversions returning a temperature trip.The value of fault queue is selectable by programming
    the two bits B4 and B3 (OS_F_QUE[1:0]) in register Conf.
  */
  typedef enum {
    eValue1 = 1,/**<Fault queue value is 1 >**/
    eValue2 = 2,/**<Fault queue value is 2>**/
    eValue3 = 4,/**<Fault queue value is 4>**/
    eValue4 = 6 /**<Fault queue value is 6>**/
  } eQueueValue_t;
  
public:
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
  DFRobot_LM75B(TwoWire *pWire = &Wire, uint8_t address = 0x48); 

  /**
   * @brief init function
   * @return Return 0 if the initialization succeeds, otherwise return non-zero and error code.
   */
  int begin();
  
  /**
   * @brief Get ambient temperature value 
   * @return Return ambient temperature value, unit: °F
   * @n Detection range: -67°F to +257°F
   */
  float getTemperatureF();
  /**
   * @brief Get ambient temperature value
   * @return Return ambient temperature, unit: °C
   * @n Detection range: -55°C to +125°C
   */
  float getTemperatureC();
  
  /**
   * @brief Get threshold temperature (Tos:Overtemperature shutdown).
   * @return Return temperature, unit: °C
   * @n Detection range: -55°C to +125°C
   */
  float getTosC(void );
  
  /**
   * @brief Get threshold temperature(Tos:Overtemperature shutdown).
   * @return Return temperature, unit: °F
   * @n Detection range: -67°F to +257°F
   */
  float getTosF(void );
  
  /**
   * @brief Get hysteresis temperature (user-defined temperature point, ≤ threshold temperature)..
   * @return Return temperature, unit: °C
   * @n Detection range: -55°C to +125°C.
   */
  float getHysteresisC();
  
  /**
   * @brief Get hysteresis temperature (user-defined temperature point,  ≤ threshold temperature)..
   * @return Return temperature, unit: °F 
   * @n Detection range: -67°F to +257°F
   */
  float getHysteresisF();
  
  /**
   * @brief Set threshold temperature (user-defined temperature point)
   * @param Tos Temperature value, unit: °C, Tos% 0.5 == 0 ；
   * @n Detection range: -55°C to +125°C
   */
  void setTosC(float Tos);
  /**
   * @brief Set threshold temperature (User-defined temperature point)
   * @param Tos Temperature, unit: °F, Tos% 0.5 == 0 ；
   * @n Detection range: -67°F to +257°F
   */
  void setTosF(float TosF);
  /**
   * @brief User-defined hysteresis temperature
   * @param Thyst Temperature, unit: °C, Thyst%0.5 == 0;
   * @n Detection range: -55°C to +125°C, Thyst must be less than Tos;
   * @n User-defined hysteresis temperature; delay the level jump of OS: OS level will jump when the
   * @n ambient temperature is less than hysteresis value instead of threshold value. 
   * @n Effect: when the temperature is more than threshold temperature, OS pin becomes active(default LOW) 
   * @n         When the temperature is less than threshold temperature, OS pin will not back to the normal 
   * @n state(default HIGH) until the temperature value is less than the hysteresis tempreature.  
   */
  void setHysteresisC(float Thyst);
  
  /**
   * @brief User-defined hysteresis temperature 
   * @param Thyst Temperature, unit: °F, Thyst%0.5 == 0 ；
   * @n Detection range: -67°F to +257°F, Thyst must be less than Tos
   */
  void setHysteresisF(float ThystF);
  
  /**
   * @brief Get fault queue value
   * @return Return fault queue value
   */
  eQueueValue_t getQueueValue();
  
  /**
   * @brief Set the value of fault queue
   * @param value eQueueValue_t type, represents the number of faluts set in the queue.
   * @n value：
   * @n Each time the temperature in the temperature register completes conversion, it will be automatically
   * @n compared with threshold and hysteresis temperature. 
   * @n eValue1, if one temperature value is more than threshold value, OS output active state; 
   * @n eValue2, if two successive temperature values are more than threshold value, OS output active state; 
   * @n eValue3, if four successive temperature values are more than threshold value, OS output active state; 
   * @n eValue4, if six successive temperature values are more than threshold value, OS output active state.
   */
  void setQueueValue(eQueueValue_t value);
  
  /**
   * @brief Get chip working mode
   * @return 0 or 1 
   * @n 0：Normal(normal mode)
   * @n 1：interrupt(shut down mode)
   */
  eShutDownMode_t getShutDownMode();
  
  /**
   * @brief Set chip working mode
   * @param ShutDownMode eQueueValue_t type, represents chip working mode
   */
  void setShutDownMode(eShutDownMode_t ShutDownMode);
  
  /**
   * @brief Get active state of OS pin (HIGH/LOW)
   * @return 0 or 1
   * @n 0：LOW(active LOW)
   * @n 1：HIGH(active HIGH)
   */
  eOSPolarityMode_t getOSPolarityMode();
  /**
   * @brief Set active state of OS pin to HIGH/LOW
   * @param polarityMode eOSPolarityMode_t type, represents polarity of OS pin
   */
  void setOSPolarityMode(eOSPolarityMode_t polarityMode);
  
  /**
   * @brief Get OS pin working mode
   * @return 0 or 1
   * @n 0：OS comparator(comparator mode)
   * @n 1：OS interrupt(interrupt mode)
   */
  eOSMode_t getOSMode();
  
  /**
   * @brief Set OS pin working mode 
   * @param OSMode eOSMode_t type, represents OS pin working mode
   */
  void setOSMode(eOSMode_t OSMode);

private:
  
  /**
   * @brief Write value into register via IIC bus 
   * @param reg  Register address 8bits
   * @param pBuf Storage cache of the data to be written into
   * @param size Length of the data to be written into
   * @return Return the actually read length, fails to read if return 0.
   */
  void writeReg(uint8_t reg, void* pBuf, size_t size) ;

  /**
   * @brief Read register function 
   * @param reg  Register address 8bits
   * @param pBuf Storage cache of the data to be read
   * @param size Length of the data to be read
   * @return Return the actually read length, fails to read if return 0.
   */
  uint8_t readReg(uint8_t reg, void* pBuf, size_t size);
private:
  TwoWire *_pWire;
  uint8_t _address;
};

#endif
