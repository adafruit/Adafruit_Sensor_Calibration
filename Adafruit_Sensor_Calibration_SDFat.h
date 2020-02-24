#ifndef __ADAFRUIT_SENSOR_CALIBRATION_SDFAT__
#define __ADAFRUIT_SENSOR_CALIBRATION_SDFAT__

#include "Adafruit_Sensor_Calibration.h"

#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT)

// Use filesystem! Must be SdFat derived
#include <SdFat.h>
// We'll need Arduino JSON to store in a file
#include <ArduinoJson.h>

#if defined(EXTERNAL_FLASH_DEVICES) || defined(PIN_QSPI_SCK)
#include <Adafruit_SPIFlash.h>
#define ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH

#if defined(EXTERNAL_FLASH_USE_QSPI) || defined(PIN_QSPI_SCK)
#define ADAFRUIT_SENSOR_CALIBRATION_USE_QSPIFLASH
#elif defined(EXTERNAL_FLASH_USE_SPI)
#define ADAFRUIT_SENSOR_CALIBRATION_USE_SPIFLASH
#else
#error("Chip has external flash chip but no interface defined in variant!")
#endif

#endif

/**!  @brief Class for managing storing calibration in external flash or
   SDFat-compatible storage **/
class Adafruit_Sensor_Calibration_SDFat : public Adafruit_Sensor_Calibration {
public:
  Adafruit_Sensor_Calibration_SDFat();
  bool begin(const char *filename = NULL, FatFileSystem *filesys = NULL);
  bool printSavedCalibration(void);
  bool loadCalibration(void);
  bool saveCalibration(void);

private:
  FatFileSystem *theFS = NULL;
  const char *_cal_filename = NULL;
  StaticJsonDocument<512> calibJSON;
};

#endif

#endif // include once
