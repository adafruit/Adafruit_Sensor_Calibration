#include "Adafruit_Sensor_Calibration.h"

/******************* When you have an external filesys (like SD) to pass in */
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT)
Adafruit_Sensor_Calibration::Adafruit_Sensor_Calibration(FatFileSystem *filesys) {
  theFS = filesys;
}
#endif

/******************* Use EEPROM or internal FLASH chip */
Adafruit_Sensor_Calibration::Adafruit_Sensor_Calibration(void) {
}


bool Adafruit_Sensor_Calibration::begin(void) {
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH)
  if (! flash.begin()) {
    return false;
  }
  Serial.print("JEDEC ID: "); Serial.println(flash.getJEDECID(), HEX);
  Serial.print("Flash size: "); Serial.println(flash.size());

  if (!fatfs.begin(&flash)) {
    Serial.println("Error, failed to mount newly formatted filesystem!");
    Serial.println("Was the flash chip formatted with the fatfs_format example?");
    return false;
  }
  theFS = &fatfs;

  Serial.println("Mounted filesystem!");
  
  File root;
  char filename[80];
  root = theFS->open("/");
  while (1) {
    File entry = root.openNextFile();
    if (!entry) {
      break; // no more files
    }
    entry.getName(filename, 80);
    Serial.print("\t");
    Serial.print(filename);
    if (entry.isDirectory()) {
      Serial.println("/");
    } else {
      // files have sizes, directories do not
      Serial.print(" : ");
      Serial.print(entry.size(), DEC);
      Serial.println(" bytes");
    }
    entry.close();
  }
  
#endif
  return true;
}

bool Adafruit_Sensor_Calibration::hasEEPROM(void) {
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM)
  return true;
#else
  return false;
#endif
}

bool Adafruit_Sensor_Calibration::hasFLASH(void) {
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH)
  return true;
#else
  return false;
#endif
}
