/***************************************************************
 * @file    SPI_Accel.ino
 * @author  7Semi,Jonathan Mejorado Lopez
 * @brief   Minimal SPI bring-up for 7Semi ICM-20948 on ESP32 +
 *          continuous accelerometer readout.
 *
 * Features
 * - SPI init on custom pins (ESP32 VSPI)
 * - IMU begin() on SPI (CS pin user-defined)
 * - Safe defaults (applyBasicDefaults)
 * - Optional sensor gating: accel only
 * - Accel config: DLPF, full-scale, ODR, DEC3
 * - Read accel (g) at ~2 Hz print
 *
* Wiring (Arduino UNO)
 * - SCK  -> GPIO13
 * - MOSI(SDA) -> GPIO12
 * - MISO(SDO) -> GPIO11
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

/** @brief SPI clock used during sensor initialization and reads. */
#define SPI_FAST_SPEED 1000000

#define CS_PIN D10  // Chip-select pin for SPI (change as needed)
/** @brief Global ICM-20948 driver instance. */
DevLab_ICM20948 imu;

/** @brief SPI bus object used by the IMU driver. */
SPIClass spi_bus(SPI);   // usa el bus SPI por defecto del Arduino

/**
 * @brief Configure the IMU for accelerometer-only operation over SPI.
 *
 * @note The sketch expects a board-level CS_PIN definition to select the
 * chip-select pin used by beginSPI().
 */
void setup() {
  Serial.begin(115200);
  delay(200);


  if (!imu.beginSPI(CS_PIN, spi_bus, 1000000)) {
    Serial.println("ERROR: beginSPI() failed");
    while (1) delay(200);
  }


  Serial.println(F("ICM-20948 ready."));

  /* Optional: gate sensors
   * - setSensors(accel_on, gyro_on, temp_on)
   * - Here: enable only accel (gyro/temp off)
   */
  if (!imu.setSensors(/*accel*/ true, /*gyro*/ false, /*temp*/ false)) {
    Serial.println(F("setSensors failed."));
  }
  /* ---------------- ACCEL DLPF Config (reference) ---------------
   * ACCEL_DLPFCFG (0..7) — 3dB & Noise Bandwidth (datasheet)
   * DLPF path ODR = 1125 / (1 + ACCEL_SMPLRT_DIV), DIV: 0..4095
   *
   * FCHOICE=0 (Bypass) : 3dB ≈ 1209 Hz, NBW ≈ 1248 Hz, Rate ≈ 4500 Hz
   * FCHOICE=1 (DLPF on):
   * - ACCEL_DLPFCFG_0 : 3dB ≈ 246.0 Hz,  NBW ≈ 265.0 Hz
   * - ACCEL_DLPFCFG_1 : 3dB ≈ 246.0 Hz,  NBW ≈ 265.0 Hz
   * - ACCEL_DLPFCFG_2 : 3dB ≈ 111.4 Hz,  NBW ≈ 136.0 Hz
   * - ACCEL_DLPFCFG_3 : 3dB ≈ 50.4  Hz,  NBW ≈ 68.8  Hz   ← good default
   * - ACCEL_DLPFCFG_4 : 3dB ≈ 23.9  Hz,  NBW ≈ 34.4  Hz
   * - ACCEL_DLPFCFG_5 : 3dB ≈ 11.5  Hz,  NBW ≈ 17.0  Hz
   * - ACCEL_DLPFCFG_6 : 3dB ≈ 5.7   Hz,  NBW ≈ 8.3   Hz
   * - ACCEL_DLPFCFG_7 : 3dB ≈ 473   Hz,  NBW ≈ 499   Hz
   */

  /* Configure accelerometer
   * AccelConfigure(DLPF, FS_SEL, dlpf_enable, dec3, stX, stY, stZ)
   * - DLPF       : ACCEL_DLPFCFG_0..7 (use 3 for balanced setting)
   * - FS_SEL     : 0..3 => ±2/±4/±8/±16 g  (g2=0, g4=1, g8=2, g16=3)
   * - dlpf_enable: true to use DLPF path (recommended)
   * - dec3       : 0..3 (DEC3_CFG averaging)
   *                0 = ACCEL_DEC3_AVG_1_OR_4  (depends on FCHOICE)
   *                1 = ACCEL_DEC3_AVG_8
   *                2 = ACCEL_DEC3_AVG_16
   *                3 = ACCEL_DEC3_AVG_32
   * - stX/Y/Z    : enable self-test (false here)
   */
  if(!imu.setAccelDLPF(ACCEL_DLPFCFG_3,false)) {
    Serial.println(F("setAccelDLPF failed."));
  }

  if(!imu.setAccelScale(ICM20948_Accel_FullScale::G_4)) {
    Serial.println(F("setAccelFS failed."));
  }
  if(!imu.setAccelAveraging(ICM20948_Accel_Average::AVG_8)) {
    Serial.println(F("setAccelAveraging failed."));
  }

  /* Set accelerometer output data rate (ODR)
   * - Accel_SMPLRT(rate_hz)
   * - Base (DLPF path) = 1125 Hz
   * - Valid range: 1–1125 Hz
   * - Example: 225 Hz 
   */
  if (!imu.setAccelDivRate(4)) { // 1125 / (1 + 4) = 225 Hz
    Serial.println(F("Accel_SMPLRT failed."));
  }
}

/**
 * @brief Read and print accelerometer data in g.
 */
void loop() {
  float ax, ay, az;  // accel X/Y/Z in g

  /* - readAccel(x,y,z)
   * - Returns:
   *   - true on success;
   * - Output:
   *   - ax/ay/az in g
   */
  if (imu.readAccel(ax, ay, az)) {
    Serial.print("ACCEL [g]: ");
    Serial.print(ax, 3);
    Serial.print(", ");
    Serial.print(ay, 3);
    Serial.print(", ");
    Serial.println(az, 3);
  } else {
    Serial.println(F("ACCEL read failed"));
  }

  Serial.println(F("-----------------------------"));
  delay(500);
}
