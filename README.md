# DevLab ICM20948 Arduino Library

Arduino driver for the module DevLab ICM-20948 9-axis IMU sensor.

This library is a DevLab adaptation derived from the 7Semi ICM20948 Arduino Library.
The original MIT license notice from 7semi-solutions is preserved in `LICENSE`.
See `NOTICE.md` for attribution and origin details.

The ICM-20948 integrates a 3-axis accelerometer, 3-axis gyroscope, and 3-axis magnetometer (AK09916), enabling motion tracking, orientation sensing, and navigation applications.

---

## Features

- 9-axis motion sensing
  - 3-axis Accelerometer
  - 3-axis Gyroscope
  - 3-axis Magnetometer

- Configurable sensor ranges
  - Accel: ±2g / ±4g / ±8g / ±16g
  - Gyro: ±250 / ±500 / ±1000 / ±2000 dps


- Digital Low Pass Filter configurable(DLPF)
  - Noise reduction
  - Configurable bandwidth

- Adjustable sample rates
  - Accel: up to 1125 Hz
  - Gyro: up to 1100 Hz

- Internal I2C Master (for magnetometer)
  - AK09916 integration via SLV0/SLV4

- ISupports both communication interfaces
  - I²C
  - SPI

- Temperature measurement

---

## Connections / Wiring

The ICM-20948 supports both **I²C** and **SPI** communication.

---

## I²C Connection

| ICM-20948 Pin | MCU Pin | Notes                     |
| ------------- | ------- | ------------------------- |
| VCC           | 3.3V    | 5V only if board supports |
| GND           | GND     | Common ground             |
| SDA           | SDA     | I²C data                  |
| SCL           | SCL     | I²C clock                 |
| AD0           | GND/3V3 | Address select            |

---

### I²C Notes

- Default I²C address: 0x29
- Supported bus speeds:
  - 100 kHz  
  - 400 kHz (recommended)  

---

## SPI Connection

| ICM-20948 Pin | MCU Pin |
| ------------- | ------- |
| VCC           | 3.3V    |
| GND           | GND     |
| SCK           | SCK     |
| MOSI          | MOSI    |
| MISO          | MISO    |
| CS            | GPIO    |

## Installation

### Arduino Library Manager

1. Open Arduino IDE  
2. Go to Library Manager  
3. Search for **DevLab ICM20948**  
4. Click Install  

---

### Manual Installation

1. Download repository as ZIP  
2. Arduino IDE → Sketch → Include Library → Add .ZIP Library  

---

## Library Overview

---

### Initialize Sensor

- I2C

```cpp
if (!imu.beginI2C(0x69, Wire, 400000))
{
  Serial.println("IMU init failed");
  while (1);
}
```

- SPI

```cpp
if (!imu.beginSPI(CS_PIN, SPI, 1000000))
{
  Serial.println("SPI init failed");
  while (1);
}
```

### Enable Sensors

```cpp
imu.setSensors(true, true, true);
```

- accel → enable accelerometer
- gyro → enable gyroscope
- temp → enable temperatur

## Reading Accelerometer

```cpp
float ax, ay, az;
imu.readAccel(ax, ay, az);
```

- Output in g

## Reading Gyroscope

```cpp
float gx, gy, gz;
imu.readGyro(gx, gy, gz);
```

- Output in degrees/sec

## Reading Temperature

```cpp
float temp;
imu.readTemperature(temp);
```

- Output in °C

## Reading Magnetometer

```cpp
imu.initMag();

float mx, my, mz;
imu.readMag(mx, my, mz);
```

- Output in microtesla (µT)

## Setting Accelerometer Scale

```cpp
imu.setAccelScale(G_4);
```

- G_2, G_4, G_8, G_16

### Setting Gyroscope Scale

```cpp
imu.setGyroScale(DPS_2000);
```

### Setting Sample Rate

```cpp
imu.setAccelSampleRate(225);
imu.setGyroSampleRate(225);
```

### DLPF Configuration

```cpp
imu.setAccelDLPF(ACCEL_DLPFCFG_3, false);
imu.setDLPF(GYRO_DLPF_3, false);
```

### Averaging Configuration

```cpp
imu.setAccelAveraging(AVG_8);
```

## Applications

- Robotics (obstacle detection)
- Drones (altitude sensing)
- Wearables (activity tracking)
- Navigation systems (IMU fusion)
- ndustrial motion sensing
- Gaming controllers

## License

- MIT License
- Original work copyright: 2025 7semi-solutions
- DevLab adaptation maintains the original license notice and attribution.
