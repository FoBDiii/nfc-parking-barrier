/*
 * ============================================================
 *  Arduino NFC Parking Barrier System
 * ============================================================
 *  Author:     Fouad Bouabdi-Benaatou
 *  Project:    BSc Applied Computing Final Year Project
 *              University of Wales Trinity Saint David, 2024-25
 *  Licence:    MIT
 *  Repository: https://github.com/YOUR_USERNAME/arduino-nfc-parking-barrier
 * ============================================================
 *
 *  HARDWARE REQUIRED:
 *    - Arduino Uno R3
 *    - RC522 NFC Module        (SPI — pins 9, 10, 11, 12, 13)
 *    - IR Proximity Sensor x2  (Digital pins 2 and 3)
 *    - Servo Motor SG90        (PWM pin 6)
 *    - 16x2 LCD + I2C Module   (I2C — A4 SDA, A5 SCL)
 *    - Breadboard + Jumper Wires
 *
 *  LIBRARIES REQUIRED (install via Arduino Library Manager):
 *    - MFRC522          by GithubCommunity (v1.4.10+)
 *    - LiquidCrystal I2C by Frank de Brabander (v1.1.2+)
 *    - Servo             built-in, no install needed
 *
 *  WIRING SUMMARY:
 *    RC522:  SDA->10, SCK->13, MOSI->11, MISO->12, RST->9, 3.3V->3.3V
 *    IR Entry: OUT->2, VCC->5V, GND->GND
 *    IR Exit:  OUT->3, VCC->5V, GND->GND
 *    Servo:    Signal->6, VCC->5V, GND->GND
 *    LCD I2C:  SDA->A4, SCL->A5, VCC->5V, GND->GND
 *
 *  WARNING: RC522 runs on 3.3V only. Connecting to 5V will
 *           permanently damage the module.
 * ============================================================
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ── Pin definitions ────────────────────────────────────────
#define SS_PIN    10    // RC522 SDA / Chip Select
#define RST_PIN    9    // RC522 Reset
#define ENTRY_IR   2    // Entry IR proximity sensor output
#define EXIT_IR    3    // Exit IR proximity sensor output
#define SERVO_PIN  6    // Servo motor PWM signal

// ── Configuration ──────────────────────────────────────────
const int MAX_CAPACITY           = 5;     // Maximum vehicles in zone
const int OPEN_ANGLE             = 90;    // Servo degrees — barrier open
const int CLOSED_ANGLE           = 0;     // Servo degrees — barrier closed
const unsigned long OPEN_DURATION = 5000; // ms barrier stays open
const unsigned long SCAN_TIMEOUT  = 10000;// ms to wait for card scan

// ── Authorised NFC card UIDs ───────────────────────────────
//
//  To find the UID of a new card:
//  1. Upload this sketch
//  2. Open Serial Monitor at 9600 baud
//  3. Scan the card — the UID will be printed
//  4. Copy the UID string into this array
//  5. Update UID_COUNT to match the number of entries
//  6. Re-upload
//
const String authorisedUIDs[] = {
  "A3F19201",
  "B4C28312",
  "C5D37423",
  "D6E48534",
  "E7F59645"
};
const int UID_COUNT = 5;

// ── Object instantiation ───────────────────────────────────
MFRC522           rfid(SS_PIN, RST_PIN);
Servo             barrierServo;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 if your LCD differs

// Current vehicle count
int occupancy = 0;

// ── Helper: UID bytes → uppercase hex string ───────────────
String getUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  return uid;
}

// ── Helper: validate UID length then check whitelist ───────
bool isAuthorised(String uid) {
  // Reject partial / noise reads (valid MIFARE UIDs are 8+ hex chars)
  if (uid.length() < 8) return false;
  for (int i = 0; i < UID_COUNT; i++) {
    if (uid == authorisedUIDs[i]) return true;
  }
  return false;
}

// ── Helper: update both LCD lines at once ──────────────────
void updateDisplay(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

// ── Helper: spaces available as string ────────────────────
String spacesStr() {
  return "Free: " + String(MAX_CAPACITY - occupancy);
}

// ── Setup ──────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  Serial.println("=== NFC Parking Barrier ===");

  // Initialise SPI and NFC reader
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RC522 initialised.");

  // Barrier starts closed
  barrierServo.attach(SERVO_PIN);
  barrierServo.write(CLOSED_ANGLE);
  Serial.println("Servo initialised — barrier closed.");

  // Initialise LCD
  lcd.init();
  lcd.backlight();
  Serial.println("LCD initialised.");

  // IR sensor pins
  pinMode(ENTRY_IR, INPUT);
  pinMode(EXIT_IR,  INPUT);

  // Ready
  updateDisplay("System Ready", spacesStr());
  Serial.println("System ready. Waiting...");
}

// ── Main loop ──────────────────────────────────────────────
void loop() {

  // ── EXIT: vehicle departing ──────────────────────────────
  if (digitalRead(EXIT_IR) == LOW) {
    if (occupancy > 0) {
      occupancy--;
      Serial.println("Exit detected. Occupancy now: " + String(occupancy));
    } else {
      Serial.println("Exit sensor triggered but occupancy already 0.");
    }
    updateDisplay("Vehicle exited", spacesStr());
    delay(2000);
    updateDisplay("Scan your card", spacesStr());
    return; // Skip entry logic this loop cycle
  }

  // ── ENTRY: vehicle at gate ───────────────────────────────
  if (digitalRead(ENTRY_IR) == LOW) {

    // Check capacity before prompting for card
    if (occupancy >= MAX_CAPACITY) {
      updateDisplay("Zone FULL", "No entry");
      Serial.println("Zone full. Entry blocked.");
      delay(3000);
      return;
    }

    updateDisplay("Scan your card", "");
    Serial.println("Vehicle detected. Waiting for card scan...");

    unsigned long startWait = millis();

    // Wait up to SCAN_TIMEOUT ms for a card to be presented
    while (millis() - startWait < SCAN_TIMEOUT) {

      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {

        String uid = getUID();
        Serial.println("Card UID detected: " + uid);

        if (isAuthorised(uid)) {
          // ── ACCESS GRANTED ──────────────────────────────
          occupancy++;
          updateDisplay("Access Granted", spacesStr());
          Serial.println("ACCESS GRANTED. Occupancy: " + String(occupancy));

          barrierServo.write(OPEN_ANGLE);
          delay(OPEN_DURATION);
          barrierServo.write(CLOSED_ANGLE);

          updateDisplay("Scan your card", spacesStr());

        } else {
          // ── ACCESS DENIED ───────────────────────────────
          updateDisplay("Access Denied", "Card not found");
          Serial.println("ACCESS DENIED. UID not in authorised list.");
          delay(2000);
          updateDisplay("Scan your card", "");
        }

        // Halt the card and stop encryption
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        break; // Exit scan-wait loop
      }
    }

    // Timeout — no card presented
    if (millis() - startWait >= SCAN_TIMEOUT) {
      Serial.println("Scan timeout. No card presented.");
      updateDisplay("Scan your card", spacesStr());
    }
  }

  delay(100); // Short delay to reduce CPU load
}
