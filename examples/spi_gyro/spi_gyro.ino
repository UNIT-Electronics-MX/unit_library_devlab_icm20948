/***************************************************************
 * @file    ICM20948_SPI_Gyro.ino
 * @brief   Minimal SPI bring-up for 7Semi ICM-20948 on ESP32/UNO +
 *          continuous gyroscope readout.
 * @author  7Semi,Jonathan Mejorado Lopez
 *
 * Features
 * - SPI init on custom pins
 * - IMU begin() on SPI (CS pin user-defined)
 * - Safe defaults (applyBasicDefaults)
 * - Optional sensor gating: gyro only
 * - Gyro config: DLPF, full-scale, ODR, AVG
 * - Read gyro (dps) at ~2 Hz print
 *
 * Wiring (Arduino UNO)
 * - SCK  -> GPIO13
 * - MOSI(SDA) -> GPIO11
 * - MISO(SDO) -> GPIO12
 * - CS   -> GPIO10 (changeable)
 * - VCC  -> 3V3
 * - GND  -> GND
 * Wiring (ESP32 VSPI default)
 * - SCK  -> GPIO18
 * - MOSI -> GPIO23
 * - MISO -> GPIO19
 * - CS   -> GPIO5 (changeable)
 * - VCC  -> 3V3
 * - GND  -> GND
 ***************************************************************/

#include <DevLab_ICM20948.h>

#define CS_PIN D10  // Chip-select pin for SPI (change as needed)

/** @brief SPI clock used during sensor initialization and reads. */
#define SPI_FAST_SPEED 1000000

/** @brief Global ICM-20948 driver instance. */
DevLab_ICM20948 imu;

/** @brief SPI bus object used by the IMU driver. */
SPIClass spi_bus(SPI);

/**
 * @brief Configure the IMU for gyroscope-only operation over SPI.
 *
 * @note The sketch expects a board-level CS_PIN definition to select the
 * chip-select pin used by beginSPI().
 */
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("ICM-20948 (SPI) — Gyro Example"));

  /* Init SPI bus on your pins. */
  // SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  if (!imu.beginSPI(CS_PIN, spi_bus, SPI_FAST_SPEED)) {
    Serial.println("ERROR: beginSPI() failed");
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
   * - DLPF     : GYRO_DLPFCFG_0..7 (use 3 for balanced setting)
   * - FS_SEL   : dps250 / dps500 / dps1000 / dps2000
   * - FCHOICE  : false → DLPF path (recommended), true → bypass (very wide BW)
   * - XGYRO/YGYRO/ZGYRO : per-axis enable (true = enable axis)
   * - AVGCFG   : 0..7 internal averaging (higher = more smoothing, more delay)
   * FCHOICE=0 (DLPF on):
   * - GYRO_DLPFCFG_0 : 3dB ≈ 12106 Hz, NBW ≈ 12316 Hz (very wide)
   * - GYRO_DLPFCFG_1 : 3dB ≈ 196.6 Hz, NBW ≈ 229.8 Hz
   * - GYRO_DLPFCFG_2 : 3dB ≈ 151.8 Hz, NBW ≈ 187.6 Hz
   * - GYRO_DLPFCFG_3 : 3dB ≈ 119.5 Hz, NBW ≈ 154.3 Hz   ← good default
   * - GYRO_DLPFCFG_4 : 3dB ≈ 51.2  Hz, NBW ≈ 73.3  Hz
   * - GYRO_DLPFCFG_5 : 3dB ≈ 23.9  Hz, NBW ≈ 35.9  Hz
   * - GYRO_DLPFCFG_6 : 3dB ≈ 5.7   Hz, NBW ≈ 8.3   Hz
   * - GYRO_DLPFCFG_7 : 3dB ≈ 473   Hz, NBW ≈ 499   Hz
   *
   * FCHOICE=1 (Bypass) : very wide (use with care; highest noise)
   */

  if(!imu.setDLPF(ICM20948_Gyro_DLPF::DLPF_51HZ, false)) {
    Serial.println(F("setGyroDLPF failed."));
  }
  if(!imu.setGyroScale(ICM20948_Gyro_FullScale::DPS_2000)) {
    Serial.println(F("setGyroScale failed."));
  }


  /* Set gyroscope output data rate (ODR)
   * - Gyro_SMPLRT(rate_hz)
   * - Base (DLPF path) = 1100 Hz
   * - Valid range: ~4.3–1100 Hz
   * - Example: 1000 Hz
   */
  if(!imu.setGyroDivRate(2)) { // 1100 / (1 + 1) = 550 Hz
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
