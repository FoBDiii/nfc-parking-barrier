# Arduino – RFID/NFC + Servo Motor

Control a servo motor with an RFID/NFC tag using an Arduino.  
Tap an authorised tag → servo rotates to **90°**. Tap again → rotates back to **0°**.

> 📖 Original tutorial: [arduinogetstarted.com](https://arduinogetstarted.com/tutorials/arduino-rfid-nfc-servo-motor)

---

## How It Works

1. User taps an RFID/NFC tag on the RC522 reader
2. Reader sends the tag's UID to Arduino over SPI
3. Arduino compares the UID against the authorised list
4. **Authorised tag** → servo toggles between 0° and 90°
5. **Unauthorised tag** → servo stays put; UID is printed to Serial Monitor

---

## Hardware Required

| Qty | Component |
|-----|-----------|
| 1 | Arduino Uno (or compatible) |
| 1 | USB Cable (Type A/B or C/B) |
| 1 | RFID/NFC RC522 Module + Tags (card & key fob) |
| 1 | Servo Motor (SG90 or similar) |
| 1 | 5V DC Power Adapter |
| 1 | DC Power Jack |
| 1 | Jumper Wires |
| 1 | Breadboard *(recommended)* |

---

## Wiring

### RFID/NFC RC522 → Arduino

| RC522 Pin | Arduino Pin |
|-----------|-------------|
| SS        | 10          |
| SCK       | 13          |
| MOSI      | 11          |
| MISO      | 12          |
| RST       | 9           |
| GND       | GND         |
| VCC       | 3.3V        |
| IRQ       | *(not connected)* |

### Servo Motor

| Servo Pin    | Connects to         |
|--------------|---------------------|
| SIG (yellow) | Arduino A5          |
| VCC (red)    | 5V adapter positive |
| GND (brown)  | 5V adapter negative |

### Power

| 5V Adapter | Connects to      |
|------------|------------------|
| Positive   | Servo VCC        |
| Positive   | Arduino Vin      |
| Negative   | Servo GND        |
| Negative   | Arduino GND      |

> ⚠️ **Voltage note:** Arduino pins output 5V; RC522 data pins are rated 3.3V.  
> For a robust build, use resistor voltage dividers or a logic-level shifter on the SPI lines.

---

## Library

Install the **MFRC522** library by *GithubCommunity* via the Arduino IDE Library Manager.

---

## Arduino Code – Single Authorised Tag

```cpp
/*
 * Arduino – RFID/NFC – Servo Motor (Single Tag)
 * Source: https://arduinogetstarted.com/tutorials/arduino-rfid-nfc-servo-motor
 * This example code is in the public domain.
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN    10
#define RST_PIN   9
#define SERVO_PIN A5

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;

byte authorizedUID[4] = {0xFF, 0xFF, 0xFF, 0xFF}; // << replace with your tag UID
int angle = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  servo.attach(SERVO_PIN);
  servo.write(angle);
  Serial.println("Tap RFID/NFC Tag on reader");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      if (rfid.uid.uidByte[0] == authorizedUID[0] &&
          rfid.uid.uidByte[1] == authorizedUID[1] &&
          rfid.uid.uidByte[2] == authorizedUID[2] &&
          rfid.uid.uidByte[3] == authorizedUID[3]) {

        Serial.println("Authorized Tag");
        angle = (angle == 0) ? 90 : 0;
        servo.write(angle);
        Serial.print("Rotate Servo Motor to ");
        Serial.print(angle);
        Serial.println("°");
      } else {
        Serial.print("Unauthorized Tag with UID:");
        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();
      }
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    }
  }
}
```

### Finding Your Tag UID

1. Upload the sketch above (keep the placeholder `0xFF` UID)
2. Open Serial Monitor at **9600 baud**
3. Tap your tag — the UID will be printed:

```
Tap RFID/NFC Tag on reader
Unauthorized Tag with UID: 3A C9 6A CB
```

4. Update line 16 with your UID:

```cpp
byte authorizedUID[4] = {0x3A, 0xC9, 0x6A, 0xCB};
```

5. Re-upload and tap your tag again

```
Tap RFID/NFC Tag on reader
Authorized Tag
Rotate Servo Motor to 90°
Authorized Tag
Rotate Servo Motor to 0°
```

---

## Arduino Code – Multiple Authorised Tags

```cpp
/*
 * Arduino – RFID/NFC – Servo Motor (Multiple Tags)
 * Source: https://arduinogetstarted.com/tutorials/arduino-rfid-nfc-servo-motor
 * This example code is in the public domain.
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN    10
#define RST_PIN   9
#define SERVO_PIN A5

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;

byte authorizedUID1[4] = {0x3A, 0xC9, 0x6A, 0xCB};
byte authorizedUID2[4] = {0x30, 0x01, 0x8B, 0x15};

int angle = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  servo.attach(SERVO_PIN);
  servo.write(angle);
  Serial.println("Tap RFID/NFC Tag on reader");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (matchUID(authorizedUID1)) {
      Serial.println("Authorized Tag 1");
      changeServo();
    } else if (matchUID(authorizedUID2)) {
      Serial.println("Authorized Tag 2");
      changeServo();
    } else {
      Serial.print("Unauthorized Tag with UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

bool matchUID(byte uid[]) {
  for (int i = 0; i < 4; i++)
    if (rfid.uid.uidByte[i] != uid[i]) return false;
  return true;
}

void changeServo() {
  angle = (angle == 0) ? 90 : 0;
  servo.write(angle);
  Serial.print("Rotate Servo Motor to ");
  Serial.print(angle);
  Serial.println("°");
}
```

---

## See Also

- [Arduino – RFID/NFC Basics](https://arduinogetstarted.com/tutorials/arduino-rfid-nfc)
- [Arduino – RFID/NFC + Relay](https://arduinogetstarted.com/tutorials/arduino-rfid-nfc-relay)
- [Arduino – RFID + Solenoid Lock](https://arduinogetstarted.com/tutorials/arduino-rfid-solenoid-lock)
- [Arduino – RFID + Electromagnetic Lock](https://arduinogetstarted.com/tutorials/arduino-rfid-electromagnetic-lock)
- [Arduino – RFID/NFC Door Lock System](https://arduinogetstarted.com/tutorials/arduino-rfid-nfc-door-lock-system)
- [Arduino – Servo Motor Basics](https://arduinogetstarted.com/tutorials/arduino-servo-motor)

---

*Tutorial content adapted from [ArduinoGetStarted.com](https://arduinogetstarted.com) for educational use.*
