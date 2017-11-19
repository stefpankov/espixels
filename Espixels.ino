#include <Adafruit_NeoPixel.h>
#include "./NeoPatterns.h"

#define NUM_OF_LEDS 60  // Number of LEDs on the Strip
#define DATA_PIN 1      // This corresponds to the GPIO10 pin on the NodeMCU ESP8266 v2 controller

// Define Strip completion callback
void StripComplete();
 
// Define a NeoPattern for our LED Strip
NeoPatterns Strip(NUM_OF_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800, &StripComplete);
 
// Initialize everything and prepare to start
void setup()
{
  Strip.begin();

  Strip.Fade(Strip.Color(200, 100, 50), Strip.Color(50, 100, 200), 50, 1000, FORWARD);
}
 
// Main loop
void loop()
{
  Strip.Update();
}
 
//------------------------------------------------------------
// Completion Routines - get called on completion of a pattern
//------------------------------------------------------------

// Strip Completion Callback
void StripComplete()
{
  // Random color change for next scan
  Strip.Color1 = Strip.Color2;
  Strip.Color2 = Strip.Wheel(random(255));
}
