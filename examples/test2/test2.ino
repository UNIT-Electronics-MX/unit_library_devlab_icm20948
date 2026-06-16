
#include <Wire.h>
#include <DevLab_ICM20948.h>

// -----------------------------------------------------------
// PINES I2C — ESP32C6 NANO Unit Electronics
// -----------------------------------------------------------
#define SDA_PIN   6
#define SCL_PIN   7
#define I2C_FREQ  400000UL
#define ICM_ADDR  0x69

// -----------------------------------------------------------
// STRUCT
// -----------------------------------------------------------
struct configDLPF {
  uint8_t     dlpf_idx;
  uint16_t    div;
  float       nbw_hz;
  float       odr_hz;
  const char* nombre;
};
//id,div,nbw_hz,odr_hz,nombre : {DLPF, NBW , ODR}
const configDLPF configsDLPF[] = {
  {  0,   0,  265.0f, 1125.0f, "246 ; 265 ; 1125 " },
  {  1,   0,  265.0f, 1125.0f, "246 ; 265 ; 1125 " },
  {  2,   1,  136.0f,  562.5f, "111 ; 136 ; 562 " },
  {  3,   2,   68.8f,  375.0f, "50 ; 69 ; 375 " },
  {  4,   7,   34.4f,  140.6f, "24 ; 34 ; 141 " },
  {  5,  15,   17.0f,   70.3f, "12 ; 17 ;  70 " },
  {  6,  29,    8.3f,   37.5f, "6 ; 8 ;  35 " },
  {  7,   0,  499.0f, 1125.0f, "473 ; 499 ; 1125 " },
};
const uint8_t N_DLPF = sizeof(configsDLPF) / sizeof(configsDLPF[0]);

// -----------------------------------------------------------
// ARRAYS ORIGINALES
// -----------------------------------------------------------
const ICM20948_Accel_FullScale accelCfg[] = {
  ICM20948_Accel_FullScale::G_2,
  ICM20948_Accel_FullScale::G_4,
  ICM20948_Accel_FullScale::G_8,
  ICM20948_Accel_FullScale::G_16
};
const char* etiquetasAccelCfg[] = { "+-2G","+-4G","+-8G","+-16G" };
const uint8_t N_FS = 4;

const ICM20948_Accel_DLPFCFG accelDLPF[] = {
  ICM20948_Accel_DLPFCFG::DLPF0_246HZ,
  ICM20948_Accel_DLPFCFG::DLPF_246HZ,
  ICM20948_Accel_DLPFCFG::DLPF_111HZ,
  ICM20948_Accel_DLPFCFG::DLPF_50HZ,
  ICM20948_Accel_DLPFCFG::DLPF_24HZ,
  ICM20948_Accel_DLPFCFG::DLPF_12HZ,
  ICM20948_Accel_DLPFCFG::DLPF_6HZ,
  ICM20948_Accel_DLPFCFG::DLPF_473HZ
};

const ICM20948_Accel_Average accelAVG[] = {
  ICM20948_Accel_Average::AVG_1_OR_4,
  ICM20948_Accel_Average::AVG_8,
  ICM20948_Accel_Average::AVG_16,
  ICM20948_Accel_Average::AVG_32
};
const char* etiquetasAccelAVG[] = { "14","8","16","32" };
const uint8_t N_AVG = 4;
const uint16_t accelSR[] = {
  0, 1, 3, 5, 7, 10, 15, 22, 31, 63, 127, 255, 513, 1022, 2044, 4095
};
const char* etiquetasSR[] = {
  "1125.0","562.5","281.3","187.5","140.6","102.3","70.3",
  "48.9","35.2","17.6","8.8","4.4","2.2","1.1","0.55","0.27"
};

// -----------------------------------------------------------
DevLab_ICM20948 imu;
uint8_t total_pass = 0;
uint8_t total_fail = 0;
uint8_t total_warn = 0;


void printLinea()      { Serial.println(F("------------------------------------------------------------")); }
void printDobleLinea() { Serial.println(F("============================================================")); }
// -----------------------------------------------------------
// WHO_AM_I
// -----------------------------------------------------------
void firstStage() {
  uint8_t who;
  if (!imu.readWhoAmI(who) || who != 0xEA) {
    Serial.println(F("ERROR: WHO_AM_I mismatch"));
    while (1) delay(200);
  }
  Serial.printf("WHO_AM_I = 0x%02X  OK\n", who);
}

// -----------------------------------------------------------
// APLICAR CONFIGURACION
//
// FIX 1: Se agregan ambas llamadas a setAccelDLPF:
//   Llamada 1 (bypass=false): escribe bits DLPFCFG, deja FCHOICE=0
//   Llamada 2 (bypass=true):  activa FCHOICE=1 sin tocar DLPFCFG
//
// FIX 2: setAccelDivRate escribe el DIV directo al registro
// -----------------------------------------------------------
bool applySettings(uint8_t fs_idx, const configDLPF &cfg,uint8_t avg) {
  uint8_t dlpf_val = (uint8_t)accelDLPF[cfg.dlpf_idx];
  bool ok = true;

  ok &= imu.setAccelScale(accelCfg[fs_idx]);


  // Paso 1: escribe DLPFCFG en bits [5:3]
  ok &= imu.setAccelDLPF(dlpf_val, false);

  ok &= imu.setAccelAveraging(accelAVG[avg]);
  // Paso 2: activa FCHOICE=1 (DLPF activo) sin tocar DLPFCFG
  //ok &= imu.setAccelDLPF(0, true);

  // DIV directo al registro, sin conversion interna
  ok &= imu.setAccelDivRate(cfg.div);

  return ok;
}

// -----------------------------------------------------------
// SWEEP — MODO VERIFICACION
//
// Por cada configuracion DLPF:
//   1. Aplica la config (FS fijo en +-2g para verificacion)
//   2. Espera settle del filtro
//   3. Imprime 3 lecturas reales
//
// Cuando las lecturas muestren Az ~1g en reposo,
// el sensor esta funcionando y se puede activar el sweep completo.
// -----------------------------------------------------------
void runSweep() {

  uint16_t n = 0;
  printDobleLinea();
  Serial.println(F("  ICM-20948 | UNIT Electronics"));
  Serial.println(F("  MODO VERIFICACION — FS=+-2g fijo, 3 lecturas por config"));
  Serial.println(F("  Sensor PLANO y QUIETO | Az esperado: ~+1.000g"));
  printDobleLinea();
  for(uint8_t i = 0; i < N_AVG; i++){
    for(uint8_t j = 0; j < N_FS;j++){
      for (uint8_t d = 0; d < N_DLPF; d++) {

        //Serial.println();
        //printLinea();
        /*Serial.printf("  Config %d/%d: %s\n", d+1, N_DLPF, configsDLPF[d].nombre);
        Serial.printf("  ODR=%.1fHz | Periodo=%.1fms\n",
                      configsDLPF[d].odr_hz,
                      1000.0f / configsDLPF[d].odr_hz);*/

        // --- FIX: aplicar la configuracion antes de leer ---
        bool cfg_ok = applySettings(j, configsDLPF[d],i);  // FS=0 → +-2g
        if (!cfg_ok) {
          Serial.println(F("  ERROR: applySettings() fallo"));
          continue;
        }

        // Esperar que el filtro estabilice (10 periodos del ODR, min 50ms)
        uint32_t settle_ms = max((uint32_t)(10000.0f / configsDLPF[d].odr_hz),
                                (uint32_t)50);
        delay(settle_ms);

        // Tomar 3 lecturas espaciadas por el periodo del ODR
        uint32_t periodo_ms = max((uint32_t)(1000.0f / configsDLPF[d].odr_hz),
                                  (uint32_t)1);
        float ax, ay, az;
        for (uint8_t r = 0; r < 100; r++) {
          n++;
          delay(periodo_ms);
          if (imu.readAccel(ax, ay, az)) {
            Serial.printf("%d ; %s ; %s ; %s ; %+.4f ; %+.4f ; %+.4f\n",
                          n,
                          configsDLPF[d].nombre,   // "246 ; 265 ; 1125"
                          etiquetasAccelCfg[i],     // "+-2G"
                          etiquetasAccelAVG[j],     // "8"
                          ax, ay, az);  
          } else {
            Serial.println(F("  [?] readAccel() fallo"));
          }
        }
      }
    }
  }
  Serial.println();
  printDobleLinea();
  Serial.println(F("  VERIFICACION COMPLETA"));
  Serial.println(F("  Si Az ~ +1.0g en todos los bloques:"));
  Serial.println(F("  el sensor funciona correctamente."));
  Serial.println(F("  Activar sweep completo cuando este listo."));
  printDobleLinea();
}

// -----------------------------------------------------------
// SETUP
// -----------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("ICM-20948 — Verificacion"));

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
  imu.sleep(false);   // ← limpia SLEEP bit, CLKSEL=1
  delay(50);

  if (!imu.setSensors(true, true, true)) {
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

// -----------------------------------------------------------
// LOOP — lectura simple post-sweep
// -----------------------------------------------------------
void loop() {
  /*float ax, ay, az;
  if (imu.readAccel(ax, ay, az)) {
    Serial.printf("X:%+.3f  Y:%+.3f  Z:%+.3f\n", ax, ay, az);
  }
  delay(500);*/
}
