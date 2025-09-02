## Lantern — Fireworks Ring, Million Sparks Strip & Smoke Machine (Arduino + FastLED + Hall Sensor)

![Lantern demo](docs/smokey_lantern.gif)

Interactive lantern using WS2812B LEDs and a smoke generator.  
Animations run continuously when a magnet is **absent** (Hall sensor detection).  
Fireworks sparkle on a 16-LED ring, a 30-LED spiral strip twinkles with evolving palettes,  
and a pump + smoke heater release bursts of smoke in sync.

### Features

* **16-LED ring** with fireworks animation (sparkles + expanding bursts).
* **30-LED spiral strip** with “Million Sparks” twinkling glitter effect.
* **Smoke cycle**: 5 s smoke + pump → 2 s pump purge → 3 s rest (non-blocking).
* **Hall sensor gating**: system only runs when magnet is absent.
* All effects are **non-blocking** for smooth animation + smoke control.

### Hardware

* **MCU**: Arduino Uno/Nano (or compatible AVR @16 MHz).
* **LEDs**: WS2812B (NeoPixel).
  * Ring: 16 LEDs, data pin **D5**.
  * Strip: 30 LEDs, data pin **D9**.
* **Smoke system**:
  * Pump (MOSFET): **D3**.
  * Smoke heater (MOSFET): **D6**.
* **Hall sensor**: **D12**.
  * Configurable polarity (`HALL_ACTIVE_LOW`).
  * Optional pull-up (`HALL_USE_PULLUP`).
* **Power**:
  * LEDs: up to ~60 mA per LED → ~2.7 A max. Provide a 5 V supply ≥3 A.
  * Smoke/pump: powered separately via MOSFETs, with **common GND**.

### Default Pinout

* Ring: **D5**  
* Strip: **D9**  
* Pump: **D3**  
* Smoke heater: **D6**  
* Hall sensor: **D12**

> Adjust these in the `#define` section at the top of the sketch.

### Wiring Tips

* Put a **330–470 Ω resistor** in series with each LED data line.  
* Add a **1000 µF capacitor** across LED 5 V and GND near the first LED.  
* Use proper MOSFETs (logic-level N-channel) to switch pump/smoke loads.  
* Add a **flyback diode** across the pump (if inductive).  
* Ensure **common ground** between Arduino, LEDs, and smoke system.

### Software Setup

1. Install the **FastLED** library (Arduino Library Manager → “FastLED”).  
2. Open the `.ino` sketch in Arduino IDE / CLI / PlatformIO.  
3. Select your board + port.  
4. Upload the firmware.

### How It Works

* The **Hall sensor** sets `systemEnabled` true/false based on magnet presence.  
  * Magnet present → system OFF (LEDs black, smoke off).  
  * Magnet absent → system ON (animations + smoke cycle run).  
* The **ring** uses a fireworks algorithm (sparkles, expanding bursts).  
* The **strip** uses a twinkle/glitter effect, with palettes that drift over time.  
* The **smoke state machine** cycles non-blocking: 5 s ON + 2 s purge + 3 s rest.  
* All updates are timed using `millis()`, no blocking `delay()` calls.

### Customization

* **Brightness**: `FastLED.setBrightness(96);`
* **Ring fireworks**: `sparkleProb`, `trailFade`, `burstProb`
* **Strip glitter**:
  * `TWINKLE_DECAY` (higher = faster fade)
  * `TWINKLE_RATE` (density of sparks)
  * `TWINKLE_WHITE_PROB` (chance of white glitter)
  * `twinkleHueDrift` (color drift speed)
* **Palettes**: rotates between Carmine–Teal, Carmine–Purple, Carmine–Ice.  
* **Hall sensor**:
  * `HALL_ACTIVE_LOW = true` → LOW = magnet present (typical modules).
  * `HALL_STABLE_COUNT` adjusts debounce/stability filter.

### Safety Notes

⚠️ **High current + heating elements**:
* Do not power LEDs or smoke system from Arduino’s 5 V pin.  
* Always use MOSFETs to switch the pump and smoke heater.  
* Keep heater away from flammable materials.  
* Supervise when smoke is active.  

### Licensing

This project follows Fab Lab–friendly open licensing:

* **Software (Arduino sketches, .ino/.cpp):** MIT License (`LICENSE`)  
* **Hardware designs (schematics, PCB, CAD):** CERN-OHL-S v2.0 (`LICENSES/CERN-OHL-S-2.0.txt`)  
* **Documentation & media (README, images):** CC BY-SA 4.0 (`LICENSES/CC-BY-SA-4.0.txt`)  

### License Summary

* **MIT**: Free use, modification, and commercial use — attribution required.  
* **CERN-OHL-S**: Hardware must remain open (strong reciprocity).  
* **CC BY-SA**: Documentation/media can be reused if credited and shared alike.  

### Acknowledgments

* [FastLED](https://fastled.io/) for the LED control library.  
* Fab Labs & the [Fab Charter](https://fabfoundation.org/about/fab-charter/) for open-knowledge principles.  

---

## NOTICE

