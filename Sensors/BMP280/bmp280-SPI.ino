/* GY-BMP280 Test Sketch to measure temperature, pressure and altitude */

#include <Wire.h> /*For I2C communication check https://www.arduino.cc/en/Reference/Wire */
#include <Adafruit_BMP280.h> /*Main library to use BMP280*/

Adafruit_BMP280 barometer;

void setup() {

  Serial.begin(9600);

  if (!barometer.begin()) {

    Serial.println(F("Could not find a connected GY-BMP280 sensor, check wiring!"));

    while (1);

  } else {

    Serial.println(F("GY-BMP280 is connected"));

  }

}

void loop() {

  Serial.print(F("Temperature (in Celcius) = "));
  Serial.println(barometer.readTemperature());

  Serial.print(F("Pressure (in Pascal) = "));
  Serial.println(barometer.readPressure());

  Serial.print(F("Pressure (in Millibar) = "));
  Serial.println(barometer.readPressure() / 100);

  Serial.print(F("Altitude approximately (in Meters) = "));
  Serial.println(barometer.readAltitude(1019.1));

  Serial.println(F(" "));
  delay(2000);

}
