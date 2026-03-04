# Troubleshooting Guide

Common problems and how to fix them.

---

## LCD Display

### Problem: LCD shows garbled characters or random symbols on startup

**Cause:** The I2C address in the code does not match your LCD module's actual address. The two most common addresses are `0x27` and `0x3F`, but some modules use others.

**Fix:**
1. Upload `i2c_scanner.ino` to your Arduino
2. Open Serial Monitor at **9600 baud**
3. The scanner will print the correct address, for example: `Found device at address: 0x27`
4. In `main.ino`, update this line to match:
   ```cpp
   LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 to your address
   ```
5. Re-upload `main.ino`

---

### Problem: LCD backlight is on but nothing is displayed

**Cause:** Contrast is set too low on the I2C module.

**Fix:** Look for a small blue potentiometer on the back of the I2C module and turn it slightly with a small screwdriver. The contrast will change as you turn it.

---

### Problem: LCD shows nothing at all — backlight is off

**Cause:** Power or wiring issue.

**Fix:**
- Check VCC is connected to **5V** (not 3.3V)
- Check GND is connected
- Check SDA → A4 and SCL → A5

---

## RC522 NFC Module

### Problem: NFC module does not detect any cards

**Cause 1:** Incorrect power connection.

**Fix:** Check the RC522 is connected to **3.3V**, not 5V. Using 5V will damage the module permanently and it will stop working entirely.

**Cause 2:** Loose SPI connection.

**Fix:** Check all five SPI wires are secure:
- SDA → pin 10
- SCK → pin 13
- MOSI → pin 11
- MISO → pin 12
- RST → pin 9

---

### Problem: Card is detected but always shows "Access Denied"

**Cause:** The UID in the code does not match the card's actual UID.

**Fix:**
1. Open Serial Monitor at **9600 baud**
2. Scan the card
3. The UID will be printed, for example: `Card UID detected: A3F19201`
4. Copy this exactly into the `authorisedUIDs[]` array in `main.ino`:
   ```cpp
   const String authorisedUIDs[] = {
     "A3F19201"  // paste your UID here
   };
   const int UID_COUNT = 1;
   ```
5. Make sure `UID_COUNT` matches the number of entries in the array
6. Re-upload

> **Note:** UIDs are case-sensitive in the comparison. The code converts all UIDs to uppercase automatically, so make sure your entries in the array are also uppercase.

---

### Problem: One unregistered card occasionally triggers the barrier briefly

**Cause:** A partial or noisy SPI read produces a short UID that coincidentally passes the comparison before the full check runs.

**Fix:** This is already handled in the code by the length check:
```cpp
if (uid.length() < 8) return false; // Reject partial reads
```
If you are still seeing this, check for loose connections on the SPI wires and make sure the RC522 GND is secure.

---

## Servo Motor

### Problem: Servo does not move at all

**Cause 1:** Wrong pin or missing library.

**Fix:**
- Confirm the signal wire is connected to **pin 6**
- Confirm `#include <Servo.h>` is at the top of the sketch
- Check the servo and Arduino share a **common GND** connection

**Cause 2:** Insufficient power.

**Fix:** If you are running the servo directly from the Arduino 5V pin, it may not have enough current, especially if the LCD and sensors are also drawing power. Try powering the Arduino from a wall USB adapter (5V 2A) rather than a laptop USB port.

---

### Problem: Servo makes a buzzing or grinding noise but does not rotate

**Cause:** The servo is mechanically blocked, or the target angles are outside the servo's range.

**Fix:**
- Make sure nothing is physically blocking the servo arm
- Check the angles in the code:
  ```cpp
  const int OPEN_ANGLE   = 90;  // Should be between 0 and 180
  const int CLOSED_ANGLE = 0;   // Should be between 0 and 180
  ```
- Adjust angles if needed for your specific barrier arm setup

---

## IR Proximity Sensors

### Problem: IR sensor always shows "detected" even with nothing in front of it

**Cause:** Sensitivity is set too high.

**Fix:** Adjust the small potentiometer on the sensor module. Turn it slowly until the onboard LED turns off when no object is present. The sensitivity is correct when the LED turns on only when an object is within a few centimetres.

---

### Problem: IR sensor never triggers even when a hand is in front of it

**Cause:** Sensitivity is set too low, or power issue.

**Fix:**
- Turn the sensitivity potentiometer in the opposite direction
- Check VCC is connected to 5V and GND is connected
- Check the OUT pin is connected to the correct Arduino digital pin (pin 2 for entry, pin 3 for exit)

---

### Problem: IR sensors work individually but cause problems when combined with other components

**Cause:** Power supply noise or shared GND issues.

**Fix:**
- Make sure all GND connections are made to the same GND rail on the breadboard
- Ensure the Arduino GND is connected to the breadboard GND rail

---

## General

### Problem: Serial Monitor shows nothing

**Fix:**
- Check the baud rate is set to **9600** in the Serial Monitor dropdown
- Make sure the correct COM port is selected under **Tools → Port** in the Arduino IDE

---

### Problem: Sketch will not upload — "port not found" error

**Fix:**
- Disconnect and reconnect the USB cable
- Check **Tools → Board** is set to **Arduino Uno**
- Try a different USB cable — some cables are charge-only and do not carry data
- On Windows, check Device Manager for the correct COM port number

---

### Problem: Libraries not found — compilation error

**Fix:** Install the missing libraries via **Sketch → Include Library → Manage Libraries**:
- Search for `MFRC522` and install by GithubCommunity
- Search for `LiquidCrystal I2C` and install by Frank de Brabander
- The Servo library is built-in and does not need installing

---

## Still stuck?

Open an issue on this repository and include:
- What you have tried
- What the Serial Monitor is showing (copy and paste the output)
- A photo of your wiring if possible
