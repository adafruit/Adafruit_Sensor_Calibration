#ifndef __ADAFRUIT_SENSOR_CALIBRATION_SDFAT__
#define __ADAFRUIT_SENSOR_CALIBRATION_SDFAT__

#include "Adafruit_Sensor_Calibration.h"

#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT)

// Use filesystem instead! Must be SdFat derived
#include <SdFat.h>
// We'll need Arduino JSON to store in a file
#include <ArduinoJson.h>
#define ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT
#endif

#if defined(EXTERNAL_FLASH_DEVICES) || defined(PIN_QSPI_SCK)
#include "Adafruit_SPIFlash.h"
#define ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH

#if defined(EXTERNAL_FLASH_USE_QSPI) || defined(PIN_QSPI_SCK)
static Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS,
                                                   PIN_QSPI_IO0, PIN_QSPI_IO1,
                                                   PIN_QSPI_IO2, PIN_QSPI_IO3);
#define ADAFRUIT_SENSOR_CALIBRATION_USE_QSPIFLASH
#elif defined(EXTERNAL_FLASH_USE_SPI)
static Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS,
                                                  &EXTERNAL_FLASH_USE_SPI);
#define ADAFRUIT_SENSOR_CALIBRATION_USE_SPIFLASH
#else
#error("Chip has external flash chip but no interface defined in variant!")
#endif

static Adafruit_SPIFlash flash(&flashTransport);
static FatFileSystem fatfs;
#endif


class Adafruit_Sensor_Calibration_SDFat : public Adafruit_Sensor_Calibration {  
  Adafruit_Sensor_Calibration_SDFat();
  bool begin(const char *filename, FatFileSystem *filesys);

private:
  FatFileSystem *theFS = NULL;
  const char *_cal_filename = NULL;
  StaticJsonDocument<512> calibJSON;
};


#endif


#endif  // include once
