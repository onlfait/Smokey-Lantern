\# Lantern — Fireworks Ring, Carmine Vortex Strip \& Smoke Machine



An Arduino project that combines \*\*WS2812B NeoPixels\*\* and a \*\*smoke generator\*\* to create a magical lantern effect:

\- A \*\*16-LED ring\*\* simulates fireworks with sparkles and expanding bursts.  

\- A \*\*30-LED strip\*\* wrapped in a spiral produces a swirling “carmine vortex” fire effect.  

\- A \*\*pump + smoke heater\*\* cycle generates bursts of real smoke in sync with the lights.  



All effects run \*\*non-blocking\*\*, so animations and smoke stay smooth.



---



\## Hardware



\- \*\*MCU:\*\* Arduino Uno (or any ATmega328-based board supported by FastLED).  

\- \*\*LEDs:\*\* WS2812B (a.k.a. NeoPixel).

&nbsp; - Ring: 16 LEDs, data pin \*\*D5\*\*.

&nbsp; - Strip: 30 LEDs, data pin \*\*D9\*\*.

\- \*\*Smoke module:\*\*

&nbsp; - Pump (via MOSFET): pin \*\*D3\*\*.

&nbsp; - Smoke heater (via MOSFET): pin \*\*D6\*\*.

\- \*\*Power:\*\*

&nbsp; - LEDs: up to ~60 mA per LED at full white → 46 LEDs = \*\*~2.7 A max\*\*.  

&nbsp;   Use a 5 V supply rated ≥3 A. Add a large electrolytic capacitor (≥1000 µF) across 5 V/GND at the LEDs.  

&nbsp;   Place a ~330–470 Ω resistor in series with each data line.

&nbsp; - Pump/smoke module: power separately through MOSFETs. Share GND with Arduino. Add a flyback diode across the pump (if inductive).



---



\## Behavior



\### Ring (D5 — 16 LEDs)

\- Fireworks effect:

&nbsp; - Sparkling embers around the ring.

&nbsp; - Occasional expanding bursts of color.



\### Strip (D9 — 30 LEDs, spiral)

\- Swirling “carmine vortex” fire:

&nbsp; - Based on the \*\*Fire2012 algorithm\*\* (heat rises).

&nbsp; - Carmine-to-teal/purple/ice palettes blend over time.

&nbsp; - Azimuth swirl makes the fire twist like a vortex.



\### Smoke Cycle (D3 pump + D6 heater)

\- Non-blocking state machine:

&nbsp; 1. \*\*5 s\*\* — Smoke heater + pump ON.

&nbsp; 2. \*\*2 s\*\* — Pump only (purges residual smoke).

&nbsp; 3. \*\*3 s\*\* — All OFF (rest).

&nbsp; 4. Repeat.



---



\## Installation



1\. Clone/download this repo.

2\. Open the `.ino` file in Arduino IDE or PlatformIO.

3\. Install \[\*\*FastLED\*\*](https://fastled.io/) via Library Manager.

4\. Connect hardware as described above.

5\. Upload to Arduino Uno (ATmega328 @16 MHz).



---



\## Tuning



\- \*\*Brightness:\*\* set in `BRIGHTNESS` (0–255).

\- \*\*Ring fireworks:\*\*

&nbsp; - `sparkleProb`, `trailFade`, `burstProb` for density/decay.

\- \*\*Strip fire physics:\*\*

&nbsp; - `COOLING` (higher = calmer),  

&nbsp; - `SPARKING` (more base sparks),  

&nbsp; - `UPDRIFT` (faster rising heat).

\- \*\*Vortex swirl:\*\* `swirlSpeed`, `swirlHueMix`.

\- \*\*Spiral smoothing:\*\* set `LEDS\_PER\_TURN` to the number of LEDs per spiral turn.  

&nbsp; If no spiral, set to 0.

\- \*\*Orientation:\*\* set `bottomIsIndex0 = true` if LED 0 is physically at the bottom.



---



\## Demo



<img src="docs/smokey\_lantern.gif">



\*Photo: Vortex effect with smoke burst.\*



---



\## Safety Notes



\*\*High current \& heating elements:\*\*

\- Do not power LEDs, pump, or smoke module from the Arduino’s 5 V pin.  

\- Use MOSFETs for switching loads, with common GND.  

\- Keep heater away from flammable material.  

\- Always supervise when smoke is active.



---



\## License



MIT — feel free to use, modify, and share.  

If you build your own lantern, please share photos or improvements!



