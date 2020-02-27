#ifndef __ADAFRUIT_SENSOR_CALIBRATION_H__
#define __ADAFRUIT_SENSOR_CALIBRATION_H__

#include <Adafruit_Sensor.h>

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__) ||              \
    defined(__AVR_ATmega2560__) || defined(ESP8266) || defined(ESP32) ||       \
    defined(TEENSYDUINO)
#define ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM
#else
#define ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT
#endif

/**************************************************************************/
/*!
    @brief A helper to manage calibration storage and retreival for
    Adafruit_Sensor events
*/
/**************************************************************************/
class Adafruit_Sensor_Calibration {

public:
  Adafruit_Sensor_Calibration(void);
  static uint16_t crc16_update(uint16_t crc, uint8_t a);

  bool hasEEPROM(void);
  bool hasFLASH(void);

  /*! @brief Overloaded calibration saving function
      @returns True if calibration was saved successfully */
  virtual bool saveCalibration(void) = 0;
  /*! @brief Overloaded calibration loading function
      @returns True if calibration was loaded successfully */
  virtual bool loadCalibration(void) = 0;
  /*! @brief Overloaded calibration printing function.
      @returns True if calibration was printed successfully */
  virtual bool printSavedCalibration(void) = 0;
  bool calibrate(sensors_event_t &event);

  /**! XYZ vector of offsets for zero-g, in m/s^2 */
  float accel_zerog[3] = {0, 0, 0};

  /**! XYZ vector of offsets for zero-rate, in rad/s */
  float gyro_zerorate[3] = {0, 0, 0};

  /**! XYZ vector of offsets for hard iron calibration (in uT) */
  float mag_hardiron[3] = {0, 0, 0};

  /**! The 3x3 matrix for soft-iron calibration (unitless) */
  float mag_softiron[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

  /**! The magnetic field magnitude in uTesla */
  float mag_field = 50;
};

#ifdef ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM
#include "Adafruit_Sensor_Calibration_EEPROM.h"
#endif

#ifdef ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT
#include "Adafruit_Sensor_Calibration_SDFat.h"
#endif

#endif
