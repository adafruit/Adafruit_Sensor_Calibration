#include "Adafruit_Sensor_Calibration.h"

#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT)

/**************************************************************************/
/*!
    @brief Instanatiate using an external Filesys (like SD?)
    @param filesys The SdFat filesystem to use
*/
/**************************************************************************/
Adafruit_Sensor_Calibration::Adafruit_Sensor_Calibration(
    FatFileSystem *filesys) {
  theFS = filesys;
}

/**************************************************************************/
/*!
    @brief Instanatiate using an EEPROM or internal FLASH chip
    @param filename The filename for the calibration, will use
   "sensor_calib.json" if NULL
*/
/**************************************************************************/
Adafruit_Sensor_Calibration::Adafruit_Sensor_Calibration(const char *filename) {
  if (filename) {
    _cal_filename = filename;
  } else {
    _cal_filename = "sensor_calib.json";
  }
}
#else

Adafruit_Sensor_Calibration::Adafruit_Sensor_Calibration(void) {}

#endif

/**************************************************************************/
/*!
    @brief Initializes Flash and filesystem
    @returns False if any failure to initialize flash or filesys
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration::begin(void) {
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH)
  if (!flash.begin()) {
    return false;
  }
  Serial.print("JEDEC ID: ");
  Serial.println(flash.getJEDECID(), HEX);
  Serial.print("Flash size: ");
  Serial.println(flash.size());

  if (!fatfs.begin(&flash)) {
    Serial.println("Error, failed to mount newly formatted filesystem!");
    Serial.println(
        "Was the flash chip formatted with the fatfs_format example?");
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

/**************************************************************************/
/*!
    @brief Save the calibration file and serialize this object's calibrations
    into JSON format
    @returns false if anything went wrong with opening the file
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration::saveCalibration(void) {

#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH)

  if (!theFS)
    return false;

  File file = theFS->open(_cal_filename, O_WRITE | O_CREAT);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return false;
  }

  JsonObject root = calibJSON.to<JsonObject>();
  JsonArray mag_hard_data = root.createNestedArray("mag_hardiron");
  for (int i = 0; i < 3; i++) {
    mag_hard_data.add(mag_hardiron[i]);
  }
  JsonArray mag_soft_data = root.createNestedArray("mag_softiron");
  for (int i = 0; i < 9; i++) {
    mag_soft_data.add(mag_softiron[i]);
  }
  JsonArray gyro_zerorate_data = root.createNestedArray("gyro_zerorate");
  for (int i = 0; i < 3; i++) {
    gyro_zerorate_data.add(gyro_zerorate[i]);
  }
  JsonArray accel_zerog_data = root.createNestedArray("accel_zerog");
  for (int i = 0; i < 3; i++) {
    accel_zerog_data.add(accel_zerog[i]);
  }
  // serializeJsonPretty(root, Serial);

  // Serialize JSON to file
  if (serializeJson(calibJSON, file) == 0) {
    Serial.println(F("Failed to write to file"));
    return false;
  }
  // Close the file (File's destructor doesn't close the file)
  file.close();

#endif

  return true;
}

/**************************************************************************/
/*!
    @brief Print the raw calibration file/EEPROM data
    @returns false if anything went wrong with opening the file
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration::printSavedCalibration(void) {
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH)
  if (!theFS)
    return false;
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
#endif
  return true;
}

/**************************************************************************/
/*!
    @brief Load the calibration file and parse JSON into this object
    @returns false if anything went wrong with opening the file
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration::loadCalibration(void) {
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH)
  if (!theFS)
    return false;

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

  for (int i = 0; i < 3; i++) {
    mag_hardiron[i] = calibJSON["mag_hardiron"][i] | 0.0;
  }
  for (int i = 0; i < 9; i++) {
    float def = 0;
    if (i == 0 || i == 4 || i == 8) {
      def = 1;
    }
    mag_softiron[i] = calibJSON["mag_softiron"][i] | def;
  }
  for (int i = 0; i < 3; i++) {
    gyro_zerorate[i] = calibJSON["gyro_zerorate"][i] | 0.0;
  }
  for (int i = 0; i < 3; i++) {
    accel_zerog[i] = calibJSON["accel_zerog"][i] | 0.0;
  }
#endif
  return true;
}

/**************************************************************************/
/*!
    @brief Calibrator that will apply known calibrations to an event
    @param  event Reference to a sensor event that we will modify in place
    @returns False if we could not calibrate this type (isn't supported)
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration::calibrate(sensors_event_t &event) {
  if (event.type == SENSOR_TYPE_MAGNETIC_FIELD) {
    // hard iron cal
    float mx = event.magnetic.x - mag_hardiron[0];
    float my = event.magnetic.y - mag_hardiron[1];
    float mz = event.magnetic.z - mag_hardiron[2];
    // soft iron cal
    event.magnetic.x =
        mx * mag_softiron[0] + my * mag_softiron[1] + mz * mag_softiron[2];
    event.magnetic.y =
        mx * mag_softiron[3] + my * mag_softiron[4] + mz * mag_softiron[5];
    event.magnetic.z =
        mx * mag_softiron[6] + my * mag_softiron[7] + mz * mag_softiron[8];
  } else if (event.type == SENSOR_TYPE_GYROSCOPE) {
    event.gyro.x -= gyro_zerorate[0];
    event.gyro.y -= gyro_zerorate[1];
    event.gyro.z -= gyro_zerorate[2];
  } else if (event.type == SENSOR_TYPE_GYROSCOPE) {
    event.acceleration.x -= accel_zerog[0];
    event.acceleration.y -= accel_zerog[1];
    event.acceleration.z -= accel_zerog[2];
  } else {
    return false;
  }
  return true;
}

/**************************************************************************/
/*!
    @brief Whether we're using EEPROM for storage
    @returns True if using EEPROM
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration::hasEEPROM(void) {
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM)
  return true;
#else
  return false;
#endif
}

/**************************************************************************/
/*!
    @brief Whether we're using SPI/QSPI Flash for storage
    @returns True if using FLASH
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration::hasFLASH(void) {
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH)
  return true;
#else
  return false;
#endif
}
