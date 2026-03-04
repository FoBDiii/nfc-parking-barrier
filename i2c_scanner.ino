/*
 * ============================================================
 *  I2C Address Scanner
 * ============================================================
 *  Use this sketch to find the I2C address of your LCD module.
 *
 *  Instructions:
 *  1. Connect the LCD I2C module to your Arduino
 *     (SDA -> A4, SCL -> A5, VCC -> 5V, GND -> GND)
 *  2. Upload this sketch
 *  3. Open Serial Monitor at 9600 baud
 *  4. The address will be printed, e.g. "Found device at 0x27"
 *  5. Use that address in main.ino:
 *       LiquidCrystal_I2C lcd(0x27, 16, 2);
 *
 *  Common LCD I2C addresses: 0x27 or 0x3F
 * ============================================================
 */

#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);

  while (!Serial); // Wait for Serial Monitor to open

  Serial.println("==============================");
  Serial.println("  I2C Address Scanner");
  Serial.println("  Scanning addresses 1-126...");
  Serial.println("==============================");

  byte devicesFound = 0;

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Found device at address: 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  <-- use this in main.ino");
      devicesFound++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  Serial.println("------------------------------");
  if (devicesFound == 0) {
    Serial.println("No I2C devices found.");
    Serial.println("Check your wiring and try again.");
  } else {
    Serial.print(devicesFound);
    Serial.println(" device(s) found.");
  }
  Serial.println("Scan complete.");
}

void loop() {
  // Nothing to do here
}
