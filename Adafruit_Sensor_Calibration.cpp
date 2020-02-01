#include "Adafruit_Sensor_Calibration.h"

/******************* When you have an external filesys (like SD) to pass in */
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT)
Adafruit_Sensor_Calibration::Adafruit_Sensor_Calibration(FatFileSystem *filesys) {
  theFS = filesys;
}
#endif

/******************* Use EEPROM or internal FLASH chip */
Adafruit_Sensor_Calibration::Adafruit_Sensor_Calibration(const char *filename) {
  if (filename) {
    _cal_filename = filename;
  } else {
    _cal_filename = "sensor_calib.json";
  }
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

bool Adafruit_Sensor_Calibration::addCalibration(adafruit_sensor_calib_t type, 
						 float *vals, uint8_t numvals) {

  return true;
}

bool Adafruit_Sensor_Calibration::saveCalibration(void) {
  if (!theFS) return false;

  File file = theFS->open(_cal_filename, O_WRITE | O_CREAT);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return false;
  }

  JsonObject root = calibJSON.to<JsonObject>();
  JsonArray mag_hard_data = root.createNestedArray("mag_hardiron");
  for (int i=0; i<3; i++) {
    mag_hard_data.add(mag_hardiron[i]);
  }
  JsonArray mag_soft_data = root.createNestedArray("mag_softiron");
  for (int i=0; i<9; i++) {
    mag_soft_data.add(mag_softiron[i]);
  }
  JsonArray gyro_zerorate_data = root.createNestedArray("gyro_zerorate");
  for (int i=0; i<3; i++) {
    gyro_zerorate_data.add(gyro_zerorate[i]);
  }
  JsonArray accel_zerog_data = root.createNestedArray("accel_zerog");
  for (int i=0; i<3; i++) {
    accel_zerog_data.add(accel_zerog[i]);
  }
  //serializeJsonPretty(root, Serial);

  // Serialize JSON to file
  if (serializeJson(calibJSON, file) == 0) {
    Serial.println(F("Failed to write to file"));
    return false;
  }
  // Close the file (File's destructor doesn't close the file)
  file.close();
  return true;
}

bool Adafruit_Sensor_Calibration::printSavedCalibration(void) {
  if (!theFS) return false;
  File file = theFS->open(_cal_filename, O_READ);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return false;
  }

  Serial.println("------------");
  while (file.available()) {
    Serial.write(file.read());
  }
  Serial.println("\n------------");
  file.close();
  yield();
}

bool Adafruit_Sensor_Calibration::loadCalibration(void) {
  if (!theFS) return false;

  File file = theFS->open(_cal_filename, O_READ);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return false;
  }

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(calibJSON, file);
  if (error) {
    Serial.println(F("Failed to read file"));
    return false;
  }

  // Close the file (File's destructor doesn't close the file)
  file.close();

  for (int i=0; i<3; i++) {
    mag_hardiron[i] = calibJSON["mag_hardiron"][i] | 0.0;  
  }
  for (int i=0; i<9; i++) {
    float def = 0;
    if (i == 0 || i == 4 || i == 8) {
      def = 1;
    }
    mag_softiron[i] = calibJSON["mag_softiron"][i] | def;  
  }
  for (int i=0; i<3; i++) {
    gyro_zerorate[i] = calibJSON["gyro_zerorate"][i] | 0.0;  
  }
  for (int i=0; i<3; i++) {
    accel_zerog[i] = calibJSON["accel_zerog"][i] | 0.0;  
  }
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
