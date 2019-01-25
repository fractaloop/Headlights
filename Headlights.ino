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
  KNIGHT_RIDER = 0,
  SOMETHING_ELSE
};

///////////////////////////////////////////////////////////////////////////////
// Globals
// Our LEDs
CRGB leds[NUM_LEDS];
// Current program
STATE currentState = KNIGHT_RIDER;
// Time tracking
long lastTime = 0;

void loopSomethingElse(long elapsed);

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

  int potValue = analogRead(A0);
  if (potValue < 1) {
    currentState = KNIGHT_RIDER;
  } else {
    currentState = SOMETHING_ELSE;
  }

  currentState = KNIGHT_RIDER;
  switch (currentState) {
    case KNIGHT_RIDER:
      loopKnightRider(elapsed);
      break;
    case SOMETHING_ELSE:
      loopSomethingElse(elapsed);
      break;
  }

  // So we don't update too crazy fast
  delay(1);
}

///////////////////////////////////////////////////////////////////////////////
// Something Else
//
//
const CHSVPalette16 somethingElsePalette(
  CHSV(0, 0, 0),            //BLACK
  CHSV(0, 0, 255),          //WHITE
  CHSV(0, 255, 255),        //RED
  CHSV(32, 255, 255),       //ORANGE  
  CHSV(64, 255, 255),       //YELLOW
  CHSV(96, 255, 255),       //GREEN
  CHSV(128, 255, 255),      //AQUA
  CHSV(160, 255, 255),      //BLUE
  CHSV(192, 255, 255),      //PURPLE
  CHSV(224, 255, 255),      //PINK
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
const Frame frames[] = {
  {1000000, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
  {1000000, {1, 0, 1, 0, 1, 0, 1, 0, 0, 0}},
  {1000000, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
};

const char numFrames = 3;
int currentFrame = 0;
int frameTime = 0;

void loopSomethingElse(long elapsed) {
  frameTime += elapsed;
  if (frameTime > frames[currentFrame].delay) {
    frameTime = 0;
    currentFrame = (currentFrame + 1) % numFrames;
  }
  // Update all the pixels from the palette
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(somethingElsePallet, frames[currentFrame].data[i] * 16);
  }

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
