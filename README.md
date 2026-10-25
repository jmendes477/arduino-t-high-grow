# LILYGO TTGO T-Higrow BME280 — Getting Started

This guide explains how to connect the T-Higrow to a computer, configure the Arduino IDE, upload a test sketch, and verify its onboard sensors.

## 1. Hardware and software

### You need

- LILYGO TTGO T-Higrow BME280 board (ESP32-based)
- USB cable that supports **data** (not charging only)
- Computer running Windows, macOS, or Linux
- Arduino IDE
- Internet connection to install board support and libraries

You do **not** need an Arduino Mega or Raspberry Pi for this initial test. The T-Higrow already contains an ESP32 microcontroller.

## 2. Connect the board

1. Connect the T-Higrow to your computer using USB.
2. The board is powered by USB.
3. The onboard sensors are connected to the ESP32 internally; no jumper wires are required for this test.

The sketch below uses these pin assignments:

| Function | ESP32 GPIO |
|---|---:|
| I²C SDA | 25 |
| I²C SCL | 26 |
| Sensor power control | 4 |
| Soil sensor analogue input | 32 |
| Additional analogue input | 34 |

**Note:** Pin assignments can vary by board revision. If a sensor is not detected or an analogue reading does not change, verify the pinout for your exact T-Higrow version.

## 3. Install Arduino IDE and ESP32 support

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Open Arduino IDE.
3. Open **Tools → Board → Boards Manager**.
4. Search for `esp32`.
5. Install **esp32 by Espressif Systems**.
6. Connect the T-Higrow.
7. Under **Tools → Board**, select an ESP32 board compatible with the module. **ESP32 Dev Module** worked for the test described here.
8. Under **Tools → Port**, select the COM port for the board (for example, `COM3` on Windows).

If no port appears:
- Try another USB data cable and USB port.
- Check Windows Device Manager for the USB-to-serial adapter and driver.
- Disconnect and reconnect the board to identify which port appears.

Some T-Higrow versions use a CH9102F USB-to-serial converter; confirm the converter on your board if a driver is needed.

## 4. Install sensor libraries

In Arduino IDE, open **Sketch → Include Library → Manage Libraries…** and install:

| Library | Publisher / note |
|---|---|
| `Adafruit BME280 Library` | Adafruit |
| `Adafruit Unified Sensor` | Adafruit |
| `BH1750` | Christopher Laws |

The `Adafruit Unified Sensor` library is required by the BME280 library. If compilation reports `Adafruit_Sensor.h: No such file or directory`, install **Adafruit Unified Sensor** through Library Manager and compile again.

## 5. Test sketch

Create a new sketch and paste this code:

```cpp
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>

// T-Higrow I2C pins
#define I2C_SDA 25
#define I2C_SCL 26

// Onboard sensor power control and soil sensor
#define POWER_CTRL 4
#define SOIL_PIN 32
#define SALT_PIN 34

Adafruit_BME280 bme;
BH1750 lightMeter;

bool bmeOK = false;
bool lightOK = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Enable power to the onboard sensors
  pinMode(POWER_CTRL, OUTPUT);
  digitalWrite(POWER_CTRL, HIGH);
  delay(200);

  // Initialise I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialise analogue readings
  analogReadResolution(12);

  // Initialise the BME280 sensor
  bmeOK = bme.begin(0x77, &Wire);

  // Initialise the BH1750 light sensor
  lightOK = lightMeter.begin(
    BH1750::CONTINUOUS_HIGH_RES_MODE
  );

  Serial.println("\\nT-Higrow BME280 test");

  Serial.print("BME280: ");
  Serial.println(bmeOK ? "OK" : "NOT DETECTED");

  Serial.print("Light sensor: ");
  Serial.println(lightOK ? "OK" : "NOT DETECTED");

  Serial.println("-------------------------");
}

void loop() {
  if (bmeOK) {
    Serial.print("Temperature: ");
    Serial.print(bme.readTemperature());
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.print("Pressure: ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");
  } else {
    Serial.println("BME280 unavailable");
  }

  if (lightOK) {
    Serial.print("Light: ");
    Serial.print(lightMeter.readLightLevel());
    Serial.println(" lux");
  } else {
    Serial.println("Light sensor unavailable");
  }

  Serial.print("Soil sensor raw value: ");
  Serial.println(analogRead(SOIL_PIN));

  Serial.print("Additional sensor raw value: ");
  Serial.println(analogRead(SALT_PIN));

  Serial.println("-------------------------");
  delay(2000);
}
```

### Important note about the sketch

This sketch assumes the BME280 I²C address is `0x77` and the onboard light sensor is compatible with the BH1750 library. If the Serial Monitor reports that either sensor is not detected, check the exact board revision and pinout. Some board revisions may use a different address or sensor configuration.

The `SALT_PIN` reading is included as an additional raw analogue value; its exact purpose depends on the board revision. Do not interpret it as a calibrated measurement without verifying the hardware documentation.

## 6. Compile and upload

1. Click **Verify** (✓) to compile.
2. Click **Upload** (→).
3. Wait for the upload to complete.

A successful upload normally ends with messages similar to:

```text
Hash of data verified.
Hard resetting via RTS pin...
```

### If upload fails

- **`No DFU capable USB device available`**: the IDE may be using an unsuitable board or upload configuration. Select a compatible ESP32 board and the serial COM port.
- **Stuck on `Connecting...`**: try holding the board's **BOOT** button while the IDE attempts to connect, then release it once upload begins.
- **No COM port**: try a known-good USB data cable and check the USB-to-serial driver.
- **Compilation error for `Adafruit_Sensor.h`**: install `Adafruit Unified Sensor` in Library Manager.

## 7. View sensor readings

1. Open **Tools → Serial Monitor**.
2. Set the baud rate to **115200**.
3. Wait for the ESP32 to restart and print readings.

Example output (illustrative only):

```text
T-Higrow BME280 test
BME280: OK
Light sensor: OK
-------------------------
Temperature: 23.40 C
Humidity: 56.20 %
Pressure: 1013.20 hPa
Light: 450.00 lux
Soil sensor raw value: 2300
Additional sensor raw value: 1800
-------------------------
```

Your values will vary depending on the environment and board. The program prints readings every two seconds.

## 8. Test each sensor

| Sensor | Test | Expected behaviour |
|---|---|---|
| Temperature | Leave the board indoors for a minute | A reasonably stable room-temperature value |
| Humidity | Compare with a room hygrometer, if available | A plausible relative-humidity percentage |
| Pressure | Observe the value for 30 seconds | A reasonably stable atmospheric pressure in hPa |
| Light | Cover and uncover the light sensor | The lux value should change |
| Soil moisture | Compare the raw reading in air and in dry/moist soil, following the board's instructions | The analogue reading should change |

### Understanding soil moisture values

The soil moisture value is a **raw analogue reading**, not a moisture percentage. Record values in known dry and wet conditions before deciding how to convert it to a percentage. Do not immerse the electronics in water, and follow the board's guidance for using the soil probe.

The BME280 measures the air around the board; it does **not** measure soil temperature.

## 9. Troubleshooting sensor readings

- **`BME280: NOT DETECTED`**: check the I²C wiring/pin definitions, sensor power control, I²C address (`0x76` or `0x77` are common), and exact board revision.
- **`Light sensor: NOT DETECTED`**: verify the light sensor model and its I²C configuration for your board revision.
- **Garbled text**: set Serial Monitor to `115200` baud.
- **No output**: press the board's **EN/RESET** button once and reopen Serial Monitor.
- **Readings never change**: check that the sensor is powered and that the correct GPIO assignments are used.

## 10. Possible next step: send readings over Wi-Fi

Once the sensors work, the next stage can be to connect the ESP32 to Wi-Fi and send measurements to a small Go API running in a home lab. A dashboard and database can then track soil moisture, temperature, humidity, pressure, and light over time.

Do this only after confirming that the local sensor readings are correct.

## References

- [Arduino IDE downloads](https://www.arduino.cc/en/software)
- [LILYGO official website](https://lilygo.cc/)
- [LILYGO T-Higrow repository](https://github.com/Xinyuan-LilyGO/LilyGO-T-HiGrow-LoRaWLAN)
- [Adafruit BME280 Library](https://github.com/adafruit/Adafruit_BME280_Library)
- [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor)
- [BH1750 library](https://github.com/claws/BH1750)
