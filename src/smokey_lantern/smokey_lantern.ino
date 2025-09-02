#include <FastLED.h>

// ========================= USER CONFIG (EDIT ME) =========================

// LED hardware configuration
#define PIN_RING       5
#define PIN_STRIP      9
#define NUM_RING       16
#define NUM_STRIP      30
#define CHIPSET        WS2812B
#define COLOR_ORDER    GRB
#define BRIGHTNESS     96   // Global brightness (0..255)

// Smoke hardware (active HIGH on MOSFET gates)
#define PIN_PUMP       3    // Air pump
#define PIN_SMOKE      6    // Smoke heating module

// Strip geometry/orientation
// If your spiral's LED 0 is physically at the TOP, set bottomIsIndex0 = false.
// This makes "heat rising" look correct visually.
bool bottomIsIndex0 = false;

// If your strip is wound as a spiral, set LEDs-per-turn here (0 disables spiral blending)
uint8_t LEDS_PER_TURN = 10;


// ========================= INTERNAL BUFFERS =========================

CRGB ring[NUM_RING];
CRGB strip[NUM_STRIP];


// ========================= RING: FIREWORKS =========================
//
// A simple fireworks effect: trailing sparkles + occasional expanding bursts.
//

// Animation cadence (ms)
const uint16_t RING_DT_MS  = 16;   // ~60 FPS
const uint16_t STRIP_DT_MS = 18;   // ~55 FPS
uint32_t tRingPrev  = 0;
uint32_t tStripPrev = 0;

// Fireworks burst entity
struct Burst {
  bool    active = false;
  uint8_t center = 0;       // LED index at ring
  uint8_t radius = 0;       // current expansion radius
  uint8_t maxRadius = 5;    // max radius
  CHSV    hue = CHSV(0,255,255);
  uint8_t fade = 180;       // not used directly here but kept for future tweaks
};

const uint8_t MAX_BURSTS = 3;
Burst bursts[MAX_BURSTS];

// Tuning for ring sparkles/fireworks
uint8_t sparkleProb = 40;  // probability of glitter spawn per frame
uint8_t sparkVmin   = 160; // min brightness for sparkles
uint8_t trailFade   = 200; // fade amount each frame (higher = more fade)
uint8_t burstProb   = 6;   // probability to spawn a new burst per frame

/// Spawn a new expanding burst if a free slot exists.
void spawnBurst() {
  for (uint8_t i = 0; i < MAX_BURSTS; i++) {
    if (!bursts[i].active) {
      bursts[i].active    = true;
      bursts[i].center    = random8(NUM_RING);
      bursts[i].radius    = 0;
      bursts[i].maxRadius = random8(3, 7);
      bursts[i].hue       = CHSV(random8(), 200, 255);
      bursts[i].fade      = random8(150, 210);
      break;
    }
  }
}

/// Update the fireworks animation on the ring.
void updateRingFireworks() {
  // Trailing fade
  fadeToBlackBy(ring, NUM_RING, trailFade);

  // Random glitter sparkles
  if (random8() < sparkleProb) {
    uint8_t idx = random8(NUM_RING);
    ring[idx] += CHSV(random8(0, 25), random8(0, 80), random8(sparkVmin, 255));
    ring[(idx + 1) % NUM_RING]            += CHSV(10, 40, 120);
    ring[(idx + NUM_RING - 1) % NUM_RING] += CHSV(10, 40, 120);
  }

  // Randomly start new bursts
  if (random8() < burstProb) spawnBurst();

  // Grow/paint existing bursts
  for (uint8_t i = 0; i < MAX_BURSTS; i++) {
    if (!bursts[i].active) continue;

    uint8_t r = bursts[i].radius, c = bursts[i].center;
    for (int8_t d = -r; d <= (int8_t)r; d++) {
      uint8_t pos     = (c + d + NUM_RING) % NUM_RING;
      uint8_t falloff = 255 - (abs(d) * (255 / (bursts[i].maxRadius + 1)));
      ring[pos] += CHSV(bursts[i].hue.h, 220, falloff);
    }

    bursts[i].radius++;
    bursts[i].hue.s = qsub8(bursts[i].hue.s, 2);   // gentle desaturation over time
    blur1d(ring, NUM_RING, 64);                    // soften edges

    if (bursts[i].radius > bursts[i].maxRadius) {
      bursts[i].active = false;
    }
  }
}


// ========================= STRIP: CARMINE VORTEX FIRE =========================
//
// A rising "heat" field (Fire2012-style) plus a rotating azimuth swirl
// mapped through carmine↔complementary color palettes.
//

// Fire “physics” tuning (strip)
uint8_t COOLING    = 40; // 20..100; higher = calmer cooling
uint8_t SPARKING   = 120; // random sparks near the base
uint8_t UPDRIFT    = 70; // heat rises; higher = faster rise
uint8_t GLOW_BOOST = 26; // subtle extra glow to avoid dark gaps behind diffuser

uint8_t heat[NUM_STRIP];

// Palettes (carmine ↔ complementary)
DEFINE_GRADIENT_PALETTE(CarmineTeal_gp){
  0,   150,  0, 20,
  60,  255, 20, 40,
  120,  20, 80,120,
  180,  10,140,160,
  255, 150,  0, 20
};
DEFINE_GRADIENT_PALETTE(CarminePurple_gp){
  0,   130,  0, 25,
  80,  255, 20, 60,
  160, 120,  0,120,
  220,  40,  0, 90,
  255, 130,  0, 25
};
DEFINE_GRADIENT_PALETTE(CarmineIce_gp){
  0,   160,  0, 20,
  90,  255, 30, 50,
  140,  10, 90,140,
  200,   0,130,180,
  255, 160,  0, 20
};

CRGBPalette16 currentPal = CarmineTeal_gp;   // start palette
CRGBPalette16 targetPal  = CarminePurple_gp; // next palette to blend toward

// Palette morphing
uint32_t    tPalPrev               = 0;
const uint16_t PAL_BLEND_DT_MS     = 25;     // how often to blend a step
uint32_t    tPalChangePrev         = 0;
const uint32_t PAL_CHANGE_EVERY_MS = 9000;   // swap target palette every 9s

// Vortex parameters (azimuth swirl)
uint8_t swirlSpeed  = 2;   // swirl angular speed
uint8_t swirlDepth  = 90;  // not used directly in finalIdx; kept for experiments
uint8_t swirlHueMix = 18;  // how much the azimuth swirl modulates palette index
uint8_t phase       = 0;   // swirl phase accumulator

/// Soft blending toward targetPal and target switching over time.
void maybeUpdatePalette() {
  uint32_t now = millis();

  if (now - tPalPrev >= PAL_BLEND_DT_MS) {
    tPalPrev = now;
    nblendPaletteTowardPalette(currentPal, targetPal, 6); // 0..48 step
  }
  if (now - tPalChangePrev >= PAL_CHANGE_EVERY_MS) {
    tPalChangePrev = now;
    uint8_t pick = random8(3);
    switch (pick) {
      case 0: targetPal = CarmineTeal_gp;   break;
      case 1: targetPal = CarminePurple_gp; break;
      default: targetPal = CarmineIce_gp;   break;
    }
  }
}

/// Optional spiral blur: mixes each LED with neighbors one full turn away.
/// Helps unify light around the lantern if the strip is wound in a spiral.
static inline void blurAlongSpiral(CRGB* leds, uint16_t nLeds, uint8_t step, uint8_t amt) {
  if (step == 0 || step >= nLeds) return;
  for (uint16_t i = 0; i < nLeds; i++) {
    uint16_t a = (i + step < nLeds) ? (i + step) : i;
    uint16_t b = (i >= step) ? (i - step) : i;
    CRGB mix = leds[i];
    nblend(mix, leds[a], amt);
    nblend(mix, leds[b], amt);
    leds[i] = mix;
  }
}

/// Update the strip’s swirling fire animation.
void updateStripVortexFire() {
  maybeUpdatePalette();

  // Advance swirl phase
  phase += swirlSpeed;

  // 1) Cooling
  for (uint8_t i = 0; i < NUM_STRIP; i++) {
    heat[i] = qsub8(heat[i], random8(0, COOLING + 1));
  }

  // 2) Updraft (heat rises)
  for (int k = NUM_STRIP - 1; k >= 2; k--) {
    uint16_t sum = heat[k - 1] + heat[k - 2] + heat[k - 2];
    heat[k] = (sum / 3);
    heat[k] = qadd8(heat[k], scale8(UPDRIFT, 255 - heat[k]));
  }

  // 3) Sparks near the base
  const uint8_t footZone = (NUM_STRIP < 5 ? NUM_STRIP : 5);
  if (random8() < SPARKING) {
    uint8_t y = random8(0, footZone);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // 4) Map heat → color with azimuth swirl modulation
  for (uint8_t j = 0; j < NUM_STRIP; j++) {
    // Height index depends on physical orientation
    uint8_t hIdx = bottomIsIndex0 ? j : (NUM_STRIP - 1 - j);

    // Spiral azimuth angle (0..255 across a full turn)
    uint8_t angle = 0;
    if (LEDS_PER_TURN > 0) {
      // use Arduino macro max() (avoid template form)
      angle = (uint8_t)((j % LEDS_PER_TURN) * (255 / max(1, LEDS_PER_TURN)));
    }

    // Swirl modulation (0..255 sine)
    uint8_t swirl = sin8(angle + phase);

    // Base heat → palette index
    uint8_t baseIndex = scale8(heat[hIdx], 240);

    // Final palette index modulated by swirl (more color life)
    uint8_t finalIdx  = qadd8(baseIndex, scale8(swirl, swirlHueMix));

    // Look up color and add subtle glow for better diffusion
    CRGB col = ColorFromPalette(currentPal, finalIdx, 255, LINEARBLEND);
    col += CHSV(finalIdx / 2, 30, GLOW_BOOST);

    strip[j] = col;

    // Slightly warmer base (physically bottom) for realism
    if (bottomIsIndex0) {
      if (j < 2) strip[j] += CRGB(30, 10, 10);
    } else {
      if (j >= NUM_STRIP - 2) strip[j] += CRGB(30, 10, 10);
    }
  }

  // 5) Blurs for a lantern-friendly blend
  blur1d(strip, NUM_STRIP, 28);
  blurAlongSpiral(strip, NUM_STRIP, LEDS_PER_TURN, 26);
}


// ========================= SMOKE STATE MACHINE =========================
//
// Non-blocking cycle:
//   1) SMOKE + PUMP ON   for FUMEE_ON_MS
//   2) PUMP ONLY         for POMPE_EXTRA_MS (purge)
//   3) ALL OFF (REST)    for FUMEE_OFF_MS
// Then repeat.
//

const unsigned long FUMEE_ON_MS    = 5000; // smoke heating + pump
const unsigned long POMPE_EXTRA_MS = 2000; // extra pump time after smoke off
const unsigned long FUMEE_OFF_MS   = 3000; // rest time

enum SmokeState { SM_IDLE, SM_SMOKE_ON, SM_PUMP_ONLY, SM_REST };
SmokeState   smState  = SM_IDLE;
unsigned long smTimer = 0;

/// Initialize smoke I/O.
void setupSmoke() {
  pinMode(PIN_PUMP,  OUTPUT);
  pinMode(PIN_SMOKE, OUTPUT);
  digitalWrite(PIN_PUMP,  LOW);
  digitalWrite(PIN_SMOKE, LOW);
}

/// Force everything OFF and reset the state machine.
void smokeAllOff() {
  digitalWrite(PIN_PUMP,  LOW);
  digitalWrite(PIN_SMOKE, LOW);
  smState = SM_IDLE;
}

/// Update smoke cycle (call every loop while running).
void updateSmoke() {
  unsigned long now = millis();
  switch (smState) {
    case SM_IDLE:
      // Start a new cycle: smoke + pump ON
      digitalWrite(PIN_PUMP,  HIGH);
      digitalWrite(PIN_SMOKE, HIGH);
      smTimer = now;
      smState = SM_SMOKE_ON;
      break;

    case SM_SMOKE_ON:
      if (now - smTimer >= FUMEE_ON_MS) {
        // Stop smoke heater, keep pump ON to purge residual smoke
        digitalWrite(PIN_SMOKE, LOW);
        smTimer = now;
        smState = SM_PUMP_ONLY;
      }
      break;

    case SM_PUMP_ONLY:
      if (now - smTimer >= POMPE_EXTRA_MS) {
        // Pump OFF → enter rest period
        digitalWrite(PIN_PUMP, LOW);
        smTimer = now;
        smState = SM_REST;
      }
      break;

    case SM_REST:
      if (now - smTimer >= FUMEE_OFF_MS) {
        // Next cycle
        digitalWrite(PIN_PUMP,  HIGH);
        digitalWrite(PIN_SMOKE, HIGH);
        smTimer = now;
        smState = SM_SMOKE_ON;
      }
      break;
  }
}


// ========================= SETUP / LOOP =========================

void setup() {
  // LEDs
  FastLED.addLeds<CHIPSET, PIN_RING,  COLOR_ORDER>(ring,  NUM_RING);
  FastLED.addLeds<CHIPSET, PIN_STRIP, COLOR_ORDER>(strip, NUM_STRIP);
  FastLED.setBrightness(BRIGHTNESS);
  fill_solid(ring,  NUM_RING, CRGB::Black);
  fill_solid(strip, NUM_STRIP, CRGB::Black);
  FastLED.show();

  // Smoke outputs
  setupSmoke();
}

void loop() {
  uint32_t now = millis();

  // Update ring fireworks ~60 FPS
  if (now - tRingPrev >= RING_DT_MS) {
    tRingPrev = now;
    updateRingFireworks();
  }

  // Update strip vortex fire ~55 FPS
  if (now - tStripPrev >= STRIP_DT_MS) {
    tStripPrev = now;
    updateStripVortexFire();
  }

  // Update smoke non-blocking state machine
  updateSmoke();

  // Push LED frame
  FastLED.show();
}
