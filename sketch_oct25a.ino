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

  Serial.println("\nT-Higrow BME280 test");

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
