#ifndef __ADAFRUIT_SENSOR_CALIBRATION_H__
#define __ADAFRUIT_SENSOR_CALIBRATION_H__

#include "Adafruit_Sensor.h"

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32u4__)
// ATmega328 and friends are too small for SD filesys, use EEPROM
#include <EEPROM.h>
#define ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM

#else

// Anything bigger? Use filesystem instead! Must be SdFat derived
#include <SdFat.h>
// We'll need Arduino JSON to store in a file
#include <ArduinoJson.h>
#define ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT
#endif

#if defined(EXTERNAL_FLASH_DEVICES) 
#include "Adafruit_SPIFlash.h"
#define ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH

#if defined(EXTERNAL_FLASH_USE_QSPI)
static Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS, PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
#define ADAFRUIT_SENSOR_CALIBRATION_USE_QSPIFLASH
#elif defined(EXTERNAL_FLASH_USE_SPI)
static Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_SPI_CS, &EXTERNAL_FLASH_USE_SPI);
#define ADAFRUIT_SENSOR_CALIBRATION_USE_SPIFLASH
#else
#error("Chip has external flash chip but no interface defined in variant!")
#endif

static Adafruit_SPIFlash flash(&flashTransport);
static FatFileSystem fatfs;

#endif


class Adafruit_Sensor_Calibration {
 public:
  Adafruit_Sensor_Calibration(void);
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT)
  Adafruit_Sensor_Calibration(FatFileSystem *filesys);
#endif
  bool begin(void);
  bool hasEEPROM(void);
  bool hasFLASH(void);

 private:
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT)
  FatFileSystem *theFS = NULL;
#endif
};
#endif
