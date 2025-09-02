<img src="docs/smokey_lantern.gif" width="400" align="center">

# Lantern — Fireworks Ring, Million Sparks Strip & Smoke Machine (Hall Sensor Controlled)

An Arduino project that combines **WS2812B NeoPixels**, a **smoke generator**, and a **Hall-effect sensor** to create an interactive magical lantern:

- A **16-LED ring** simulates fireworks with sparkles and expanding bursts.  
- A **30-LED strip** wrapped in a spiral twinkles like **“a thousand sparks”**, using evolving color palettes.  
- A **pump + smoke heater** cycle generates bursts of real smoke.  
- A **Hall-effect sensor** detects a magnet:  
  - **Magnet present → system OFF** (LEDs black, pump + smoke off).  
  - **Magnet absent → system ON** (animations + smoke cycle start).  

All effects run **non-blocking**, so transitions are smooth and responsive.

---

## Hardware

- **MCU:** Arduino Uno (or ATmega328-based board).  
- **LEDs:** WS2812B (NeoPixel).
  - Ring: 16 LEDs, data pin **D5**.
  - Strip: 30 LEDs, data pin **D9**.
- **Smoke system:**
  - Pump (via MOSFET): pin **D3**.
  - Smoke heater (via MOSFET): pin **D6**.
- **Hall sensor:** pin **D12**.  
  - Configure polarity with `HALL_ACTIVE_LOW`.  
  - Use `HALL_USE_PULLUP = true` if your module requires internal pull-up.
- **Power:**
  - LEDs: up to ~60 mA per LED @ full white → 46 LEDs ≈ **2.7 A max**.  
    Provide a 5 V supply rated ≥3 A.  
    Add a 1000 µF capacitor across LED supply + a 330–470 Ω resistor on data lines.
  - Pump + smoke module: power separately with MOSFETs and shared GND.  
    Add a flyback diode across the pump if it’s inductive.

---

## Behavior

### Ring (D5 — 16 LEDs)
- Fireworks animation:
  - Sparkles that shimmer randomly.
  - Bursts expand outward with fading trails.

### Strip (D9 — 30 LEDs, spiral)
- “Million Sparks” animation:
  - Continuous twinkling across the strip.
  - Color drift through carmine ↔ teal ↔ purple ↔ ice palettes.
  - Occasional bright white flashes simulate glitter.

### Smoke Cycle (D3 pump + D6 heater)
- Non-blocking cycle:
  1. **5 s** — Smoke heater + pump ON.  
  2. **2 s** — Pump only (purge).  
  3. **3 s** — All OFF (rest).  
  4. Repeat while system is active.

### Hall Sensor (D12)
- **Magnet present → OFF**  
  - LEDs black.  
  - Pump and smoke disabled.  
  - No CPU load (no FastLED.show calls).  
- **Magnet removed → ON**  
  - Animations + smoke cycle resume immediately.  

This makes the lantern interactive: hide the magnet inside the lid to “lock” the lantern, and remove it to “awaken” the fire.

---

## Installation

1. Clone/download this repo.
2. Open the `.ino` file in Arduino IDE or PlatformIO.
3. Install [**FastLED**](https://fastled.io/).
4. Connect hardware as described above.
5. Upload to Arduino Uno (ATmega328 @16 MHz).

---

## Tuning

- **Brightness:** `BRIGHTNESS` (0–255).
- **Ring fireworks:** `sparkleProb`, `trailFade`, `burstProb`.
- **Strip twinkle:**  
  - `TWINKLE_DECAY`: higher = sparks fade faster.  
  - `TWINKLE_RATE`: more sparks per frame.  
  - `TWINKLE_WHITE_PROB`: chance of white glitter.  
  - `twinkleHueDrift`: how quickly base hue drifts.
- **Palettes:** blends every 9 seconds between Carmine–Teal, Carmine–Purple, Carmine–Ice.
- **Spiral blur:** set `LEDS_PER_TURN` to actual LEDs per spiral turn (0 disables).  
- **Orientation:** set `bottomIsIndex0 = true` if LED 0 is at the bottom physically.
- **Hall sensor:**  
  - `HALL_ACTIVE_LOW = true`: LOW = magnet present (common modules).  
  - Increase `HALL_STABLE_COUNT` if your sensor is noisy.

---

## Demo

<img src="docs/lantern-hall-demo.jpg" width="400">

*Photo: “Million Sparks” effect active, triggered by magnet removal.*

---

## Safety Notes

⚠️ **High current & heating elements:**  
- Never power LEDs, pump, or smoke module directly from Arduino’s 5 V pin.  
- Always use MOSFETs for switching loads.  
- Common GND required between power supplies.  
- Keep smoke heater away from flammable objects.  
- Supervise operation while smoke is active.

---

## License

MIT — free to use, modify, and share.  
If you build your own version, feel free to share improvements or photos!
