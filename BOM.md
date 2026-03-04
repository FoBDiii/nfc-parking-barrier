# Bill of Materials

Full parts list for the Arduino NFC Parking Barrier System.

All components are available from Amazon UK, eBay, or specialist electronics suppliers such as The Pi Hut, Pimoroni, or RS Components.

---

## Components

| Component | Specification | Quantity | Approx. Cost | Notes |
|---|---|---|---|---|
| Arduino Uno R3 | ATmega328P, 14 digital I/O pins, 6 analogue pins | 1 | £18 | The official Arduino or a compatible clone both work |
| RC522 NFC Module | 13.56 MHz, MIFARE, SPI interface | 1 | £4 | **Runs on 3.3V only — do not connect to 5V** |
| IR Proximity Sensor | Digital output, adjustable sensitivity potentiometer | 2 | £3 | Usually sold in packs of 5 — keep spares |
| Servo Motor | SG90, 180° rotation, 4.8–6V operating voltage | 1 | £4 | Any standard SG90-compatible servo works |
| 16×2 LCD Display + I2C Module | HD44780 controller, I2C backpack (PCF8574), address 0x27 or 0x3F | 1 | £5 | Buy the version with the I2C module already soldered on |
| Small Adhesive Breadboard | 400-point or 830-point | 1 | £3 | The adhesive backing holds it in place |
| Jumper Wires Male-to-Male | 20 cm standard dupont | 1 pack | £1.50 | Needed for Arduino ↔ breadboard connections |
| Jumper Wires Male-to-Female | 20 cm standard dupont | 1 pack | £1.50 | Needed for sensors and servo (which have female connectors) |

---

## Total Cost

| | |
|---|---|
| **Minimum (clone Arduino, no extras)** | ~£30 |
| **Typical build** | ~£40 |
| **With spare components** | ~£50 |

---

## Optional Extras (for a more permanent installation)

| Component | Purpose | Approx. Cost |
|---|---|---|
| USB power adapter (5V 2A) | Power the Arduino without a laptop | £5 |
| Project enclosure box | Protect the electronics | £8 |
| Barrier arm (wooden dowel or plastic rod) | Physical barrier attached to servo | £2 |
| Longer jumper wires (30 cm) | More flexible layout | £2 |

---

## Where to Buy (UK)

- **Amazon UK** — fastest delivery, all components available
- **eBay UK** — cheapest option, allow extra delivery time
- **The Pi Hut** — [thepihut.com](https://thepihut.com) — good quality, UK-based
- **Pimoroni** — [pimoroni.com](https://pimoroni.com) — UK-based, good stock
- **RS Components** — [rs-online.com](https://rs-online.com) — professional supplier, higher quality
- **Arduino Store** — [store.arduino.cc](https://store.arduino.cc) — official Arduino Uno
