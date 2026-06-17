/***************************************************************
 * @file    I2C_Gyro.ino
 * @author  7Semi,Jonathan Mejorado Lopez 
 * @brief   Minimal I2C bring-up for 7Semi ICM-20948 on ESP32/UNO +
 *          continuous gyroscope readout.
 *
 * Features
 * - I2C init on default/custom pins
 * - IMU begin() on I2C (addr 0x68/0x69)
 * - Safe defaults (applyBasicDefaults)
 * - Optional sensor gating: gyro only
 * - Gyro config: DLPF, full-scale, ODR, AVG
 * - Read gyro (dps) at ~2 Hz print
 *
 * Wiring (Arduino UNO, I2C)
 * - SDA  -> A4
 * - SCL  -> A5
 * - VCC  -> 3V3 (or 5V if your board supports it)
 * - GND  -> GND
 *
 * Wiring (ESP32, I2C)
 * - SDA  -> GPIO21 (default)
 * - SCL  -> GPIO22 (default)
 * - VCC  -> 3V3
 * - GND  -> GND
 *
 * Address
 * - Default ICM-20948 I2C address is 0x68 (AD0=LOW). If AD0=HIGH, use 0x69.
 ***************************************************************/

#include <Wire.h>
#include <DevLab_ICM20948.h>

/* ====================== User Config =======================
 * - Edit pins/addr if needed
 * - UNO uses fixed SDA=A4, SCL=A5 (no need to call Wire.begin with pins)
 * - ESP32 can use default Wire (SDA=21, SCL=22) or custom pins via Wire.begin(sda,scl)
 */

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
 * @brief Configure the IMU for gyroscope-only operation over I2C.
 *
 * The function initializes the I2C interface, applies safe defaults, enables
 * the gyroscope, and configures DLPF, full-scale range, and output data rate.
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

  /* Apply safe defaults. */
  if (!imu.applyBasicDefaults()) {
    Serial.println(F("ERROR: applyBasicDefaults() failed."));
    while (1) delay(200);
  }

  Serial.println(F("ICM-20948 ready."));

  /* Optional: gate sensors
   * - setSensors(accel_on, gyro_on, temp_on)
   * - Here: enable only gyro (accel/temp off)
   */
  if (!imu.setSensors(/*accel*/ false, /*gyro*/ true, /*temp*/ false)) {
    Serial.println(F("setSensors failed."));
  }

  /* ----------------- GYRO DLPF Config (reference) ----------------
   * GyroConfig(DLPFCFG, FS_SEL, FCHOICE, XGYRO, YGYRO, ZGYRO, AVGCFG)
   * - DLPF     : GYRO_DLPFCFG_0..7 (use 3 or 4 for balanced settings)
   * - FS_SEL   : dps250 / dps500 / dps1000 / dps2000
   * - FCHOICE  : false → DLPF path (recommended), true → bypass (very wide BW)
   * - XGYRO/YGYRO/ZGYRO : per-axis enable (true = enable axis)
   * - AVGCFG   : 0..7 internal averaging (higher = more smoothing, more delay)
   * FCHOICE=0 (DLPF on):
   * - GYRO_DLPFCFG_1 : 3dB ≈ 196.6 Hz, NBW ≈ 229.8 Hz
   * - GYRO_DLPFCFG_2 : 3dB ≈ 151.8 Hz, NBW ≈ 187.6 Hz
   * - GYRO_DLPFCFG_3 : 3dB ≈ 119.5 Hz, NBW ≈ 154.3 Hz    
   * - GYRO_DLPFCFG_4 : 3dB ≈ 51.2  Hz, NBW ≈ 73.3  Hz
   * - GYRO_DLPFCFG_5 : 3dB ≈ 23.9  Hz, NBW ≈ 35.9  Hz  ← good default
   * - GYRO_DLPFCFG_6 : 3dB ≈ 5.7   Hz, NBW ≈ 8.3   Hz
   * - GYRO_DLPFCFG_7 : 3dB ≈ 473   Hz, NBW ≈ 499   Hz
   *
   * FCHOICE=1 (Bypass) : very wide (use with care; highest noise)
   */
  if(!imu.setDLPF(ICM20948_Gyro_DLPF::DLPF_23HZ, false)) {
    Serial.println(F("setDLPF failed."));
  }
  /* Set gyroscope full-scale range. */
  if(!imu.setGyroScale(ICM20948_Gyro_FullScale::DPS_2000)) {
    Serial.println(F("setGyroScale failed."));
  }

  /* Set gyroscope output data rate (ODR)
   * - Gyro_DIV_RATE = 1100 / (1 + DIV_RATE)
   * - DIV_RATE = 0..255
   * - Example: DIV_RATE=5 → ODR ≈ 183.33 Hz
   */
  if (!imu.setGyroDivRate(5)) { // 1100 / (1 + 5) = 183.33 Hz
    Serial.println(F("setGyroDivRate failed."));
  }
}

/**
 * @brief Read and print gyroscope data in degrees per second.
 */
void loop() {
  float gx, gy, gz;  // gyro X/Y/Z in dps

  /* - readGyro(x,y,z)
   * - Returns:
   *   - true on success;
   * - Output:
   *   - gx/gy/gz in dps
   */
  if (imu.readGyro(gx, gy, gz)) {
    Serial.print("GYRO [dps]: ");
    Serial.print(gx, 3);
    Serial.print(", ");
    Serial.print(gy, 3);
    Serial.print(", ");
    Serial.println(gz, 3);
  } else {
    Serial.println(F("GYRO read failed"));
  }

  Serial.println(F("-----------------------------"));
  delay(500);
}
