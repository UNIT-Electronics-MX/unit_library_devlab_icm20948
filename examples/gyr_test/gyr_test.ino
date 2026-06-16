/**
 * @file gyr_test.ino
 * @brief Gyroscope DLPF and full-scale sweep for the 7Semi ICM-20948.
 * @details Applies each gyroscope filter/full-scale configuration over I2C and
 * prints repeated samples for validation and noise comparison.
 */

#include <Wire.h>
#include <DevLab_ICM20948.h>
// -----------------------------------------------------------
// PINES I2C — ESP32C6 NANO Unit Electronics
// -----------------------------------------------------------
/** @brief I2C SDA pin used by the example board. */
#define SDA_PIN   6
/** @brief I2C SCL pin used by the example board. */
#define SCL_PIN   7
/** @brief I2C bus speed in hertz. */
#define I2C_FREQ  400000UL
/** @brief ICM-20948 I2C address selected by the AD0 pin. */
#define ICM_ADDR  0x69

// -----------------------------------------------------------
// STRUCT
// -----------------------------------------------------------
/** @brief Gyroscope DLPF divider and timing configuration. */
struct configDLPF {
  /** @brief Index into the gyroDLPF lookup table. */
  uint8_t     dlpf_idx;
  /** @brief Sample-rate divider written to the IMU register. */
  uint16_t    div;
  /** @brief Noise bandwidth in hertz for reporting. */
  float       nbw_hz;
  /** @brief Output data rate in hertz for timing calculations. */
  float       odr_hz;
  /** @brief Text label printed with each captured sample. */
  const char* nombre;
};
//id,div,nbw_hz,odr_hz,nombre : {DLPF, NBW , ODR}
/*
/** @brief Gyroscope DLPF configurations exercised by the sweep. */
const configDLPF configsDLPF[] = {
  {  0,   0,  229.8f, 1125.0f, "196,6 ; 229,8 ; 1125.0 " },
  {  1,   0,  187.6f, 1125.0f, "151,8 ; 187.6 ; 1125.0 " },
  {  2,   0,  154.3f,  1125.0f, "119,5 ; 154,3 ; 1125.0 " },
  {  3,   2,   73.3f,  375.0f, "51,2 ; 73,3 ; 375 " },
  {  4,   5,   35.9f,  187.5f, "23,9 ; 35,9 ; 187.5 " },
  {  5,  14,   17.8f,   75.0f, "11,6 ; 17,8 ;  75.0 " },
  {  6,  29,    8.9f,   37.5f, "5,7 ; 8,9 ;  37.5 " },
  {  7,   0,  376.5f, 1125.0f, "361,4 ; 376,5 ; 1125 " },
};*/
const configDLPF configsDLPF[] = {
  {  0,   0,  229.8f, 1125.0f, "196,6 ; 229,8 ; 1125.0 " },
  {  1,   0,  187.6f, 1125.0f, "151,8 ; 187.6 ; 1125.0 " },
  {  2,   0,  154.3f,  1125.0f, "119,5 ; 154,3 ; 1125.0 " },
  {  3,   2,   73.3f,  375.0f, "51,2 ; 73,3 ; 375 " },
  {  4,   5,   35.9f,  187.5f, "23,9 ; 35,9 ; 187.5 " },
  {  5,  14,   17.8f,   75.0f, "11,6 ; 17,8 ;  75.0 " },
  {  6,  29,    8.9f,   37.5f, "5,7 ; 8,9 ;  37.5 " },
  {  7,   0,  376.5f, 1125.0f, "361,4 ; 376,5 ; 1125 " },
};
/** @brief Number of gyroscope DLPF configurations. */
const uint8_t N_DLPF = sizeof(configsDLPF) / sizeof(configsDLPF[0]);

/** @brief Gyroscope full-scale ranges exercised by the sweep. */
const ICM20948_Gyro_FullScale gyroFSCfg[] = {
  ICM20948_Gyro_FullScale::DPS_250,
  ICM20948_Gyro_FullScale::DPS_500,
  ICM20948_Gyro_FullScale::DPS_1000,
  ICM20948_Gyro_FullScale::DPS_2000
};

/** @brief Text labels matching gyroFSCfg. */
const char* etiquetasGyroFSCfg[] = {"250", "500", "1000", "2000"};
/** @brief Number of gyroscope full-scale ranges. */
const uint8_t N_FS = 4;

/** @brief Gyroscope DLPF enum values matching configsDLPF. */
const ICM20948_Gyro_DLPF gyroDLPF[] = {
  ICM20948_Gyro_DLPF::DLPF_196HZ,
  ICM20948_Gyro_DLPF::DLPF_151HZ,
  ICM20948_Gyro_DLPF::DLPF_119HZ,
  ICM20948_Gyro_DLPF::DLPF_51HZ,
  ICM20948_Gyro_DLPF::DLPF_23HZ,
  ICM20948_Gyro_DLPF::DLPF_11HZ,
  ICM20948_Gyro_DLPF::DLPF_5HZ,
  ICM20948_Gyro_DLPF::DLPF_361HZ
};

/** @brief Gyroscope averaging settings available for experiments. */
const ICM20948_Gyro_Average gyroAVGCfg[]= {
  ICM20948_Gyro_Average::AVG_1,
  ICM20948_Gyro_Average::AVG_2,
  ICM20948_Gyro_Average::AVG_4,
  ICM20948_Gyro_Average::AVG_8,
  ICM20948_Gyro_Average::AVG_16,
  ICM20948_Gyro_Average::AVG_32,
  ICM20948_Gyro_Average::AVG_64,
  ICM20948_Gyro_Average::AVG_128
};     

/** @brief Text labels matching gyroAVGCfg. */
const char* etiquetasGyroAVGCfg[] = {"1","2","4","8","16","32","64","128"};
/** @brief Number of gyroscope averaging settings. */
const uint8_t N_AVG = 8;

/** @brief Global ICM-20948 driver instance. */
DevLab_ICM20948 imu;

/** @brief Print a separator line to Serial. */
void printLinea()      { Serial.println(F("------------------------------------------------------------")); }
/** @brief Print a double separator line to Serial. */
void printDobleLinea() { Serial.println(F("============================================================")); }

/**
 * @brief Verify the IMU identity register.
 */
void firstStage() {
  uint8_t who;
  if (!imu.readWhoAmI(who) || who != 0xEA) {
    Serial.println(F("ERROR: WHO_AM_I mismatch"));
    while (1) delay(200);
  }
  Serial.printf("WHO_AM_I = 0x%02X  OK\n", who);
}

/**
 * @brief Apply one gyroscope full-scale and DLPF configuration.
 *
 * @param fs_idx Index into gyroFSCfg.
 * @param cfg DLPF timing and divider configuration.
 * @return true if all configuration writes succeeded, otherwise false.
 */
bool applySettings(uint8_t fs_idx, const configDLPF &cfg) {
  ICM20948_Gyro_DLPF dlpf_val = gyroDLPF[cfg.dlpf_idx];
  bool ok = true;

  ok &= imu.setGyroScale(gyroFSCfg[fs_idx]);


  // Paso 1: escribe DLPFCFG en bits [5:3]
  // SetDLPF false -> Activa DLPF 
  // SetDLPF true -> Desactiva DLPF
  ok &= imu.setDLPF(dlpf_val, false);

  //ok &= imu.setGyroAveraging(gyroAVGCfg[avg]);
  // Paso 2: activa FCHOICE=1 (DLPF activo) sin tocar DLPFCFG
  //ok &= imu.setAccelDLPF(0, true);

  // DIV directo al registro, sin conversion interna
  ok &= imu.setGyroDivRate(cfg.div);

  return ok;
}

/**
 * @brief Run the gyroscope validation sweep and print each captured sample.
 */
void runSweep(){
  uint16_t n = 0;
  printDobleLinea();
  Serial.println(F("  ICM-20948 | UNIT Electronics"));
  Serial.println(F("  MODO VERIFICACION — FS=+-2g fijo, 3 lecturas por config"));
  Serial.println(F("  Sensor PLANO y QUIETO | Az esperado: ~+1.000g"));
  printDobleLinea();
  for (uint8_t i = 0; i < N_FS ; i++){
      for(uint8_t d = 0; d < N_DLPF; d++){

        bool cfg_ok = applySettings(i,configsDLPF[d]);
        if(!cfg_ok){
          Serial.println(F("ERROR: applySettings fallo"));
        }

        // Esperar que el filtro estabilice (10 periodos del ODR, min 50ms)
        uint32_t settle_ms = max((uint32_t)(10000.0f / configsDLPF[d].odr_hz),
                                (uint32_t)50);
        delay(settle_ms);
        // Tomar 3 lecturas espaciadas por el periodo del ODR
        uint32_t periodo_ms = max((uint32_t)(1000.0f / configsDLPF[d].odr_hz),
                                  (uint32_t)1);
        float gx, gy, gz;
        for (uint8_t r = 0; r < 100; r++) {
          n++;
          delay(periodo_ms);
          if (imu.readGyro(gx, gy, gz)) {
            Serial.printf("%d ; %s ; %s ; %+.4f ; %+.4f ; %+.4f\n",
                          n,
                          configsDLPF[d].nombre,   // "246 ; 265 ; 1125"
                          etiquetasGyroFSCfg[i],     // "+-2G"
                          //etiquetasGyroAVGCfg[j],     // "8"
                          gx, gy, gz);  
          } else {
            Serial.println(F("  [?] readGyr() fallo"));
          }
      }
    }
  
  }
}

/**
 * @brief Initialize I2C, reset/wake the IMU, enable gyro, and start the sweep.
 */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(200);

    printDobleLinea();
  Serial.println(F("  ICM-20948 VALIDACION ACELEROMETRO"));
  printDobleLinea();

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!imu.beginI2C(ICM_ADDR, Wire, I2C_FREQ)) {
    Serial.println(F("ERROR: beginI2C() fallo"));
    while (1) delay(200);
  }

  firstStage();

  // softReset pone el chip a dormir
  // Despues del reset hay que despertar el dispositivo
  imu.softReset();
  delay(100);
  imu.sleep(false);   // limpia SLEEP bit, CLKSEL=1
  delay(50);

  if (!imu.setSensors(false, true, false)) {
    Serial.println(F("ERROR: setSensors() fallo"));
  }

  Serial.println(F("  Sensor listo.\n"));
  Serial.println(F("  Sensor PLANO y QUIETO sobre la mesa."));
  Serial.println(F("  Iniciando en 5 segundos..."));
  for (int i = 5; i > 0; i--) {
    Serial.printf("  %d...\n", i);
    delay(1000);
  }

  //for (int runs = 0; runs < 20 ; runs++){
    runSweep();
  //}
}

/**
 * @brief Empty loop; this sketch runs the gyroscope sweep once in setup().
 */
void loop() {
  // put your main code here, to run repeatedly:

}
