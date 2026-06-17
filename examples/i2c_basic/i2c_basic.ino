/***************************************************************
 * @file    I2C_Basic.ino
 * @author  7Semi,Jonathan Mejorado Lopez 
 * @brief   Minimal I2C bring-up for 7Semi ICM-20948 +
 *          basic Accel/Gyro/Temp/Mag readout (updated API).
 *
 * Features
 * - I2C init (UNO / ESP32)
 * - beginI2C() initialization
 * - Manual sensor enable (setSensors)
 * - Read Accel / Gyro / Temp / Mag
 *
 * Notes
 * - WHO_AM_I must be 0xEA
 * - Sensors must be explicitly enabled
 * - Magnetometer requires initMag()
 *
 * Wiring (Arduino UNO I2C)
 * - SDA -> A4
 * - SCL -> A5
 * - VCC -> 3V3 (or 5V if supported)
 * - GND -> GND
 *
 * Wiring (ESP32 default I2C)
 * - SDA -> GPIO21
 * - SCL -> GPIO22
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
 * @brief Configure Serial, I2C, sensor identity check, and enabled sensors.
 *
 * The sketch stops in an infinite loop if the IMU cannot be initialized or
 * the WHO_AM_I register does not match the expected ICM-20948 value.
 */
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("ICM-20948 — I2C Basic"));
  Wire.begin(SDA_PIN, SCL_PIN);
  /* Initialize IMU. */
  if (!imu.beginI2C(ICM_ADDR, Wire, 400000)) {
    Serial.println(F("ERROR: beginI2C() failed"));
    while (1) delay(200);
  }

  /* WHO_AM_I check. */
  uint8_t who;
  if (!imu.readWhoAmI(who) || who != 0xEA) {
    Serial.println(F("ERROR: WHO_AM_I mismatch"));
    while (1) delay(200);
  }

  Serial.print(F("WHO_AM_I = 0x"));
  Serial.println(who, HEX);

  /* Enable all sensors. */
  if (!imu.setSensors(true, true, true)) {
    Serial.println(F("ERROR: setSensors failed"));
  }

  /* Initialize magnetometer. */
  if (!imu.initMag()) {
    Serial.println(F("Mag init failed"));
  } else {
    Serial.println(F("Mag initialized"));
  }

  Serial.println(F("Ready.\n"));
}

/**
 * @brief Read accelerometer, gyroscope, magnetometer, and temperature data.
 *
 * Values are printed periodically to the Serial monitor in engineering units.
 */
void loop() {
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;
  float tC;

  /* Read accelerometer. */
  if (imu.readAccel(ax, ay, az)) {
    Serial.print(F("ACC [g]: "));
    Serial.print(ax, 3); Serial.print(", ");
    Serial.print(ay, 3); Serial.print(", ");
    Serial.println(az, 3);
  } else {
    Serial.println(F("ACC read failed"));
  }

  /* Read gyroscope. */
  if (imu.readGyro(gx, gy, gz)) {
    Serial.print(F("GYR [dps]: "));
    Serial.print(gx, 2); Serial.print(", ");
    Serial.print(gy, 2); Serial.print(", ");
    Serial.println(gz, 2);
  } else {
    Serial.println(F("GYR read failed"));
  }

  /* Read magnetometer. */
  if (imu.readMag(mx, my, mz)) {
    Serial.print(F("MAG [uT]: "));
    Serial.print(mx, 2); Serial.print(", ");
    Serial.print(my, 2); Serial.print(", ");
    Serial.println(mz, 2);
  } else {
    Serial.println(F("MAG read failed"));
  }

  /* Read temperature. */
  if (imu.readTemperature(tC)) {
    Serial.print(F("TMP [C]: "));
    Serial.println(tC, 2);
  } else {
    Serial.println(F("TMP read failed"));
  }

  Serial.println(F("-----------------------------"));
  delay(500);
}

/* Note on magnetometer:
 * - Uses internal I2C master (AK09916 via ICM20948)
 * - initMag() must be called before readMag()
 * - If values are zero:
 *   → check initMag()
 *   → verify power + pull-ups
 */
