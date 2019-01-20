#include <stdint.h>
#include <FastLED.h>

///////////////////////////////////////////////////////////////////////////////
// Hardware config
#define LED_PIN     9
#define NUM_LEDS    10
#define LED_TYPE    NEOPIXEL

///////////////////////////////////////////////////////////////////////////////
// Types

// Basic state machine for different programs
typedef enum STATE {
  KNIGHT_RIDER = 0
};

///////////////////////////////////////////////////////////////////////////////
// Globals
// Our LEDs
CRGB leds[NUM_LEDS];
// Current program
STATE currentState = KNIGHT_RIDER;
// Time tracking
long lastTime = 0;

///////////////////////////////////////////////////////////////////////////////
// LET'S DO THIS THING!
void setup() {
  // power-up safety delay
  delay( 3000 );

  Serial.begin(9600);
  // initialization
  FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS);

  lastTime = micros();
}

void loop() {
  long currentTime = micros();
  long elapsed = (currentTime - lastTime);
  lastTime = currentTime;

  switch (currentState) {
    case KNIGHT_RIDER:
      loopKnightRider(elapsed);
      break;
  }

  // So we don't update too crazy fast
  delay(1);
}

///////////////////////////////////////////////////////////////////////////////
// Knight Rider
// Kitt is your copilot
//
// This works by dimming all current pixels and setting the current pixel to
// the brightest in the palette

// How many microseconds to go across
const float krSpeed = 1000000;
// Dimming rate. Number of microseconds to dim 1 unit
const float krDimTime = 20;
// The colors to use
// Note: Heat Colors is only from 0-240
const CRGBPalette16 krPalette = HeatColors_p;
// Maximum color index for the "hot" pixel
const uint8_t krMaxColor = 128;

// Current phase
float krPhase = 0.0f;
// Time since the last dimming
long krDimmedAt = 0;
// Current direction
boolean isForward = true;

// All the palette indices
uint8_t krPixels[NUM_LEDS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void loopKnightRider(long elapsed) {
  // Update dimming
  krDimmedAt += elapsed;
  if (krDimmedAt > krDimTime) {
    krDimmedAt = 0;

    // Dim all the pixels
    for (int i = 0; i < NUM_LEDS; i++) {
      krPixels[i] = max(0, krPixels[i] - 1);
    }
  }

  // Update phase pixel
  if (isForward) {
    krPhase += elapsed / krSpeed;
    // Switch directions
    if (krPhase > 1) {
      krPhase -= krPhase - 1;
      isForward = false;
    }
  } else {
    krPhase -= elapsed / krSpeed;
    // Switch directions
    if (krPhase < 0) {
      krPhase = -krPhase;
      isForward = true;
    }
  }

  // TODO Make this even smoother by interpolating the "current pixel"
  uint8_t currentPixel = (int)(krPhase * NUM_LEDS);
  krPixels[currentPixel] = krMaxColor;

  // Update all the pixels from the palette
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(krPalette, krPixels[i]);
  }

  // Show it
  FastLED.show();
}
