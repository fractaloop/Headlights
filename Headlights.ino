#include <stdint.h>
#include <FastLED.h>

///////////////////////////////////////////////////////////////////////////////
// Hardware config
#define LED_PIN     9
#define NUM_LEDS    24
#define LED_TYPE    NEOPIXEL

///////////////////////////////////////////////////////////////////////////////
// Types

// Basic state machine for different programs
typedef enum STATE {
  KNIGHT_RIDER = 0,
  PATTERN_FLASHER = 1,
  STATIC_RUNNING_LIGHTS = 2
};

///////////////////////////////////////////////////////////////////////////////
// Globals
// Our LEDs
CRGB leds[NUM_LEDS];
// Current program
STATE currentState = KNIGHT_RIDER;
// Time tracking
long lastTime = 0;
int analogPin = A0;
int potValue = 0;
void loopPatternFlasher(long elapsed);

///////////////////////////////////////////////////////////////////////////////
// LET'S DO THIS THING!
void setup() {
  // power-up safety delay
  delay( 1000 );

  Serial.begin(9600);
  // initialization
  FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS);

  lastTime = micros();
}

void loop() {
  long currentTime = micros();
  long elapsed = (currentTime - lastTime);
  lastTime = currentTime;

  potValue = analogRead(analogPin);
  if (potValue < 256) {
    currentState = STATIC_RUNNING_LIGHTS;
  } else {
    currentState = PATTERN_FLASHER;
  }

  switch (currentState) {
    case KNIGHT_RIDER:
      loopKnightRider(elapsed);
      break;
    case PATTERN_FLASHER:
      loopPatternFlasher(elapsed);
      break;
    case STATIC_RUNNING_LIGHTS:
      loopStaticRunningLights(elapsed);
      break;  
  }

  // So we don't update too crazy fast
  delay(1);
}

///////////////////////////////////////////////////////////////////////////////
// PATTERN FLASHER
//
//
const CHSVPalette16 BasicPalette(
  CHSV(0, 0, 0),            //  0  BLACK
  CHSV(0, 0, 255),          //  1  WHITE
  CHSV(0, 255, 255),        //  2  RED
  CHSV(32, 255, 255),       //  3  ORANGE
  CHSV(64, 255, 255),       //  4  YELLOW
  CHSV(96, 255, 255),       //  5  GREEN
  CHSV(128, 255, 255),      //  6  AQUA
  CHSV(160, 255, 255),      //  7  BLUE
  CHSV(192, 255, 255),      //  8  PURPLE
  CHSV(224, 255, 255),      //  9  PINK
  CHSV(0, 255, 255),
  CHSV(0, 255, 255),
  CHSV(0, 255, 255),
  CHSV(0, 255, 255),
  CHSV(0, 255, 255),
  CHSV(0, 255, 255)

);

typedef struct Frame {
  long delay;
  uint8_t data[NUM_LEDS];
};

// Remember, delay is in microseconds!!!
const Frame pattern1[] = {
  {100,    {0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}},
  {70000,  {2, 2, 7, 7, 3, 3, 0, 0, 0, 0, 2, 7, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}},
  {70000,  {0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}},
  {70000,  {2, 2, 7, 7, 3, 3, 0, 0, 0, 0, 2, 7, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}},
  {200000, {0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}},
  {70000,  {0, 0, 0, 0, 3, 3, 7, 7, 2, 2, 0, 0, 1, 1, 1, 1, 1, 1, 7, 2, 1, 1, 1, 1}},
  {70000,  {0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}},
  {70000,  {0, 0, 0, 0, 3, 3, 7, 7, 2, 2, 0, 0, 1, 1, 1, 1, 1, 1, 7, 2, 1, 1, 1, 1}},
  {200000, {0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}}
};

const char numFrames = 9;
int currentFrame = 0;
long frameTime = 0;

void loopPatternFlasher(long elapsed) {
  frameTime += elapsed;
  if (frameTime > pattern1[currentFrame].delay) {
    frameTime = 0;
    currentFrame = (currentFrame + 1) % numFrames;
  }
  // Update all the pixels from the palette
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(BasicPalette, pattern1[currentFrame].data[i] * 16);
  }

  // Show it
  FastLED.show();
}


////////////////////////////////////////////////////////////////////////////////////////
//Static Running Lights
//

const Frame fixedpattern[] = {
  {100,    {2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3}}

};

void loopStaticRunningLights(long elapsed) {
  frameTime += elapsed;
  if (frameTime > fixedpattern[currentFrame].delay) {
    frameTime = 0;
    currentFrame = (currentFrame + 1) % numFrames;
  }
  // Update all the pixels from the palette
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(BasicPalette, fixedpattern[0].data[i] * 16);
  }
  FastLED.setBrightness(potValue);
  // Show it
  FastLED.show();
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
const CRGBPalette16  krPalette = HeatColors_p;
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
