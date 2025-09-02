<img src="docs/smokey_lantern.gif">
#Lantern — Fireworks Ring, Carmine Vortex Strip & Smoke Machine

An Arduino project that combines **WS2812B NeoPixels** and a **smoke generator** to create a magical lantern effect:
- **16-LED ring** simulates fireworks with sparkles and expanding bursts.  
- **30-LED strip** wrapped in a spiral produces a swirling “carmine vortex” fire effect.  
- **pump + smoke heater** cycle generates bursts of real smoke in sync with the lights.  

All effects run **non-blocking**, so animations and smoke stay smooth.

## Hardware
- **MCU:** Arduino Uno (or any ATmega328-based board supported by FastLED).  
- **LEDs:** WS2812B (a.k.a. NeoPixel).
  - Ring: 16 LEDs, data pin **D5**.
  - Strip: 30 LEDs, data pin **D9**.

- **Smoke module:**
 - Pump (via MOSFET): pin **D3**.
 - Smoke heater (via MOSFET): pin **D6**.

- **Power:**
 - LEDs: up to ~60 mA per LED at full white → 46 LEDs = **~2.7 A max**.  
   Use a 5 V supply rated ≥3 A. Add a large electrolytic capacitor (≥1000 µF) across 5 V/GND at the LEDs.  
   Place a ~330–470 Ω resistor in series with each data line.
 - Pump/smoke module: power separately through MOSFETs. Share GND with Arduino. Add a flyback diode across the pump (if inductive).

## Behavior
### Ring (D5 — 16 LEDs)
- Fireworks effect:
 - Sparkling embers around the ring.
 - Occasional expanding bursts of color.

### Strip (D9 — 30 LEDs, spiral)
- Swirling “carmine vortex” fire:
 - Based on the **Fire2012 algorithm** (heat rises).
 - Carmine-to-teal/purple/ice palettes blend over time.
 - Azimuth swirl makes the fire twist like a vortex.
 - 
### Smoke Cycle (D3 pump + D6 heater)
- Non-blocking state machine:
 1. **5 s** — Smoke heater + pump ON.
 2. **2 s** — Pump only (purges residual smoke).
 3. **3 s** — All OFF (rest).
 4. Repeat.

## Installation

1. Clone/download this repo.
2. Open the `.ino` file in Arduino IDE or PlatformIO.
3. Install [**FastLED**](https://fastled.io/) via Library Manager.
4. Connect hardware as described above.
5. Upload to Arduino Uno (ATmega328 @16 MHz).

## Tuning
- **Brightness:** set in `BRIGHTNESS` (0–255).
- **Ring fireworks:**
 - `sparkleProb`, `trailFade`, `burstProb` for density/decay.
- **Strip fire physics:**
 - `COOLING` (higher = calmer),  
 - `SPARKING` (more base sparks),  
 - `UPDRIFT` (faster rising heat).
- **Vortex swirl:** `swirlSpeed`, `swirlHueMix`.
- **Spiral smoothing:** set `LEDS_PER_TURN` to the number of LEDs per spiral turn.  
 If no spiral, set to 0.
- **Orientation:** set `bottomIsIndex0 = true` if LED 0 is physically at the bottom.

## Safety Notes
**High current & heating elements:**
- Do not power LEDs, pump, or smoke module from the Arduino’s 5 V pin.  
- Use MOSFETs for switching loads, with common GND.  
- Keep heater away from flammable material.  
- Always supervise when smoke is active.

## License
MIT — feel free to use, modify, and share.  
If you build your own lantern, please share photos or improvements!



