/**
 * @file interrupt.ino
 * @brief I2C interrupt example for the 7Semi ICM-20948.
 * @details Configures the IMU raw-data-ready interrupt pin and counts ISR
 * events on an Arduino-compatible board.
 */

#include <Wire.h>
#include <DevLab_ICM20948.h>

/** @brief I2C SDA pin used by the example board. */
#define SDA_PIN   6
/** @brief I2C SCL pin used by the example board. */
#define SCL_PIN   7
/** @brief I2C bus speed in hertz. */
#define I2C_FREQ  400000UL
/** @brief ICM-20948 I2C address selected by the AD0 pin. */
#define ICM_ADDR  0x69
/** @brief MCU pin connected to the IMU interrupt output. */
#define PIN_INT   D7   // ajusta a tu pin real

/** @brief Global ICM-20948 driver instance. */
DevLab_ICM20948 imu;

/** @brief Print a separator line to Serial. */
void printLinea()      { Serial.println(F("------------------------------------------------------------")); }
/** @brief Print a double separator line to Serial. */
void printDobleLinea() { Serial.println(F("============================================================")); }

/** @brief Interrupt pin electrical and latch configuration. */
ICM20948_IntPinConfig pinCfg = {
  .activeLevel   = 1,
  .driveMode     = 0,
  .latchMode     = 0,
  .clearMode     = 1,
  .fsyncActLevel = 0,
  .fsyncIntEn    = 0,
  .bypassEn      = 0
};

/** @brief Interrupt enable flags used by this example. */
ICM20948_IntEnableConfig intEn = {
  .rawDataRdyEn = 1   // inicializacion directa
};

/** @brief Number of interrupt events observed by the ISR. */
volatile uint32_t isrCount = 0;

/**
 * @brief Interrupt service routine for the IMU data-ready signal.
 */
void IRAM_ATTR isrHandler() {
  isrCount++;
}

/**
 * @brief Initialize I2C, verify the IMU, configure interrupts, and attach ISR.
 */
void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!imu.beginI2C(ICM_ADDR, Wire, I2C_FREQ)) {
    Serial.println(F("ERROR: beginI2C() failed."));
    while (1) delay(200);
  }
  Serial.println(F("ICM-20948 ready."));

  uint8_t who;
  if (!imu.readWhoAmI(who) || who != 0xEA) {
    Serial.println(F("ERROR: WHO_AM_I mismatch"));
    while (1) delay(200);
  }
  Serial.printf("WHO_AM_I = 0x%02X\n", who);

  if(!imu.intInit(pinCfg)){
    Serial.println(F("ERROR: Initialization interruption failed"));
    while(1) delay(200);
  };           // nombre correcto
  if(!imu.intEnableConfig(intEn)){
    Serial.println(F("ERROR: Initialization interruption failed"));
    while(1) delay(200);
  }

  attachInterrupt(digitalPinToInterrupt(PIN_INT), isrHandler, FALLING);
  Serial.println(F("Interrupt configured. Waiting..."));
}

/**
 * @brief Report interrupt events when the ISR count changes.
 */
void loop() {
  static uint32_t lastCount = 0;

  if (isrCount != lastCount) {
    lastCount = isrCount;
    Serial.printf(">>> ISR disparada! Total: %lu\n", isrCount);
  }
}
