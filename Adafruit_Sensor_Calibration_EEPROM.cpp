#include "Adafruit_Sensor_Calibration.h"

#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM)

/**************************************************************************/
/*!
    @brief Initializes Flash and filesystem
    @returns False if any failure to initialize flash or filesys
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration_EEPROM::begin(uint8_t eeprom_addr) {
  ee_addr = eeprom_addr;

#if defined(ESP8266) || defined(ESP32)
  EEPROM.begin(512);
#endif

  return true;
}

bool Adafruit_Sensor_Calibration_EEPROM::saveCalibration(void) {
  Serial.println("Save Cal");

  uint8_t buf[EEPROM_CAL_SIZE];
  memset(buf, 0, EEPROM_CAL_SIZE);
  buf[0] = 0x75;
  buf[1] = 0x54;

  float offsets[16];
  memcpy(offsets, accel_zerog, 12);       // 3 x 4-byte floats
  memcpy(offsets + 3, gyro_zerorate, 12); // 3 x 4-byte floats
  memcpy(offsets + 6, mag_hardiron, 12);  // 3 x 4-byte floats

  offsets[9] = mag_field;

  offsets[10] = mag_softiron[0];
  offsets[11] = mag_softiron[4];
  offsets[12] = mag_softiron[8];
  offsets[13] = mag_softiron[1];
  offsets[14] = mag_softiron[2];
  offsets[15] = mag_softiron[5];

  memcpy(buf + 2, offsets, 16 * 4);

  uint16_t crc = 0xFFFF;
  for (uint16_t i = 0; i < EEPROM_CAL_SIZE - 2; i++) {
    crc = crc16_update(crc, buf[i]);
  }
  Serial.print("CRC: ");
  Serial.println(crc, HEX);
  buf[EEPROM_CAL_SIZE - 2] = crc & 0xFF;
  buf[EEPROM_CAL_SIZE - 1] = crc >> 8;

  for (uint16_t a = 0; a < EEPROM_CAL_SIZE; a++) {
    EEPROM.write(a + ee_addr, buf[a]);
  }

#if defined(ESP8266) || defined(ESP32)
  EEPROM.commit();
#endif
  return true;
}

bool Adafruit_Sensor_Calibration_EEPROM::loadCalibration(void) {
  uint8_t buf[EEPROM_CAL_SIZE];

  uint16_t crc = 0xFFFF;
  for (uint16_t a = 0; a < EEPROM_CAL_SIZE; a++) {
    buf[a] = EEPROM.read(a + ee_addr);
    crc = crc16_update(crc, buf[a]);
  }

  if (crc != 0 || buf[0] != 0x75 || buf[1] != 0x54) {
    Serial.print("CRC: ");
    Serial.println(crc, HEX);
    return false;
  }

  float offsets[16];
  memcpy(offsets, buf + 2, 16 * 4);
  accel_zerog[0] = offsets[0];
  accel_zerog[1] = offsets[1];
  accel_zerog[2] = offsets[2];

  gyro_zerorate[0] = offsets[3];
  gyro_zerorate[1] = offsets[4];
  gyro_zerorate[2] = offsets[5];

  mag_hardiron[0] = offsets[6];
  mag_hardiron[1] = offsets[7];
  mag_hardiron[2] = offsets[8];

  mag_field = offsets[9];

  mag_softiron[0] = offsets[10];
  mag_softiron[1] = offsets[13];
  mag_softiron[2] = offsets[14];
  mag_softiron[3] = offsets[13];
  mag_softiron[4] = offsets[11];
  mag_softiron[5] = offsets[15];
  mag_softiron[6] = offsets[14];
  mag_softiron[7] = offsets[15];
  mag_softiron[8] = offsets[12];

  return true;
}

bool Adafruit_Sensor_Calibration_EEPROM::printSavedCalibration(void) {
  Serial.println(F("------------"));
  for (uint16_t a = ee_addr; a < ee_addr + EEPROM_CAL_SIZE; a++) {
    uint8_t c = EEPROM.read(a);
    Serial.print("0x");
    if (c < 0x10)
      Serial.print('0');
    Serial.print(c, HEX);
    Serial.print(", ");
    if ((a - ee_addr) % 16 == 15) {
      Serial.println();
    }
  }
  Serial.println(F("\n------------"));
  return true;
}

#endif
