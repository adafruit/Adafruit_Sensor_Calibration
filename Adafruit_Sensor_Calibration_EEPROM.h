#ifndef __ADAFRUIT_SENSOR_CALIBRATION_EEPROM__
#define __ADAFRUIT_SENSOR_CALIBRATION_EEPROM__

#include "Adafruit_Sensor_Calibration.h"

#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM)

#include <EEPROM.h>

#define EEPROM_CAL_SIZE 68

/**!  @brief Class for managing storing calibration in internal EEPROM memory
 * **/
class Adafruit_Sensor_Calibration_EEPROM : public Adafruit_Sensor_Calibration {
public:
  bool begin(uint8_t eeprom_addr = 60);

  bool saveCalibration(void);
  bool loadCalibration(void);
  bool printSavedCalibration(void);

private:
  uint16_t ee_addr = 0;
};

#endif

#endif // include once
