/***************************************************************
 * @file    I2C_Magneto.ino
 * @author  7Semi,Jonathan Mejorado Lopez
 * @brief   Minimal I2C bring-up for 7Semi ICM-20948 +
 *          AK09916 magnetometer readout (updated API).
 *
 * Features
 * - I2C init on default/custom pins
 * - IMU beginI2C() initialization
 * - Manual sensor enable
 * - Magnetometer initialization (initMag)
 * - Read magnetometer (uT) at ~2 Hz
 *
 * Wiring (Arduino UNO, I2C)
 * - SDA  -> A4
 * - SCL  -> A5
 * - VCC  -> 3V3 (or 5V if board supports it)
 * - GND  -> GND
 *
 * Wiring (ESP32, I2C default)
 * - SDA  -> GPIO21
 * - SCL  -> GPIO22
 * - VCC  -> 3V3
 * - GND  -> GND
 *
 * Notes
 * - WHO_AM_I must be 0xEA
 * - initMag() must be called before readMag()
 * - Magnetometer runs via internal I2C master
 *
 * Author  : 7Semi 
 * License : MIT
 ***************************************************************/
#include <Wire.h>
#include <DevLab_ICM20948.h>

/* ====================== User Config ======================= */
/** @brief I2C SDA pin used by the example board. */
#define SDA_PIN   6
/** @brief I2C SCL pin used by the example board. */
#define SCL_PIN   7
/** @brief I2C bus speed in hertz. */
#define I2C_FREQ  400000UL
/** @brief ICM-20948 I2C address selected by the AD0 pin. */
#define ICM_ADDR  0x69

/** @brief Global ICM-20948 driver instance. */
DevLab_ICM20948 imu;

/**
 * @brief Configure I2C, verify the IMU, and initialize the magnetometer.
 *
 * The magnetometer is accessed through the ICM-20948 internal I2C master, so
 * initMag() must succeed before loop() can read magnetic field data.
 */
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("ICM-20948 (I2C) — Magnetometer Example"));
  Wire.begin(SDA_PIN, SCL_PIN);
  /* Initialize IMU. */
  if (!imu.beginI2C(ICM_ADDR, Wire, 400000)) {
    Serial.println(F("ERROR: beginI2C() failed."));
    while (1) delay(200);
  }

  Serial.println(F("ICM-20948 ready (I2C)."));

  /* Verify device. */
  uint8_t who;
  if (!imu.readWhoAmI(who) || who != 0xEA) {
    Serial.println(F("ERROR: WHO_AM_I mismatch"));
    while (1) delay(200);
  }

  Serial.print(F("WHO_AM_I = 0x"));
  Serial.println(who, HEX);

  /* Enable required sensors (mag uses internal I2C master)
   * - accel/gyro not strictly required but safe to keep ON
   */
  if (!imu.setSensors(true, true, false)) {
    Serial.println(F("setSensors failed."));
  }

  /* Initialize magnetometer. */
  if (!imu.initMag()) {
    Serial.println(F("ERROR: initMag() failed"));
    while (1) delay(200);
  }

  Serial.println(F("Magnetometer ready"));
}

/**
 * @brief Read and print magnetometer data in microtesla.
 */
void loop() {
  float mx, my, mz;

  /* Read magnetometer data
   * - Output in microtesla (uT)
   * - Returns true on success
   */
  if (imu.readMag(mx, my, mz)) {
    Serial.print(F("MAG [uT]: "));
    Serial.print(mx, 2); Serial.print(F(", "));
    Serial.print(my, 2); Serial.print(F(", "));
    Serial.println(mz, 2);
  } else {
    Serial.println(F("Mag read failed"));
  }

  Serial.println(F("-----------------------------"));
  delay(500);
}
