#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "./NeoPatterns.h"

#define NUM_OF_LEDS 60  // Number of LEDs on the Strip
#define DATA_PIN 1      // This corresponds to the GPIO10 pin on the NodeMCU ESP8266 v2 controller

// Define WiFi id and password
const char *ssid = "";
const char *password = "";

// Initialize the server
ESP8266WebServer server(80);

// Define function to connect to WiFi
void ConnectToWiFi();
// Define Strip completion callback
void StripComplete();

// Define route handlers
void HandleRoot();
void HandleFade();
void HandleRainbow();
void HandleNotFound();

// Define a NeoPattern for our LED Strip
NeoPatterns Strip(NUM_OF_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800, &StripComplete);

unsigned long lastUpdate;

// Initialize everything and prepare to start
void setup()
{
  Strip.begin();

  Strip.Fade(Strip.Color(200, 100, 50), Strip.Color(50, 100, 200), 50, 400, FORWARD);
//  Strip.RainbowCycle(500, FORWARD);

  ConnectToWiFi();
//  MDNS.begin("esp8266")

  server.on("/", HandleRoot);
  server.on("/fade", HandleFade);
  server.on("/rainbow", HandleRainbow);

  server.onNotFound(HandleNotFound);
//
  server.begin();
}

// Main loop
void loop()
{
  server.handleClient();
  Strip.Update();
}

//------------------------------------------------------------
// Completion Routines - get called on completion of a pattern
//------------------------------------------------------------

// Strip Completion Callback
void StripComplete()
{
  if (Strip.ActivePattern == FADE) {
    // Random color change for next scan
    Strip.Color1 = Strip.Color2;
    Strip.Color2 = Strip.Wheel(random(255));
  }

  if (Strip.ActivePattern == RAINBOW_CYCLE) {
    Strip.Color1 = Strip.Wheel(random(255));
  }
}

//------------------------------------------------------------
// Function that handles connecting to WiFi
//------------------------------------------------------------

void ConnectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

//------------------------------------------------------------
// Server route handlers
//------------------------------------------------------------

void HandleRoot()
{
  digitalWrite(LED_BUILTIN, LOW); // Turn LED on
  server.send(200, "application/json", "{ \"message\": \"Welcome to Espixels\" }");
  digitalWrite(LED_BUILTIN, HIGH); // Turn LED off
}

void HandleFade()
{
  digitalWrite(LED_BUILTIN, LOW); // Turn LED on
  Strip.Fade(Strip.Color(200, 100, 50), Strip.Color(50, 100, 200), 50, 400, FORWARD);
  server.send(200, "application/json", "{ \"message\": \"Fade started\" }");
  digitalWrite(LED_BUILTIN, HIGH); // Turn LED off
}

void HandleRainbow()
{
  digitalWrite(LED_BUILTIN, LOW); // Turn LED on
  Strip.RainbowCycle(400, FORWARD);
  server.send(200, "application/json", "{ \"message\": \"Rainbow started\" }");
  digitalWrite(LED_BUILTIN, HIGH); // Turn LED off
}

void HandleNotFound()
{
  digitalWrite(LED_BUILTIN, LOW); // Turn LED on
  String message = "{\"message\":\"Not Found\"";
  message += ",\"uri\": ";
  message += "\"" + server.uri() + "\"";
  message += ",\"method\":";
  message += (server.method() == HTTP_GET) ? "\"GET\"" : "\"POST\"";
  message += ",\"arguments\":";
  message += "{";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    if (i == server.args() - 1) {
      message += "\"" + server.argName(i) + "\":\"" + server.arg(i) + "\"";
    } else {
      message += "\"" + server.argName(i) + "\":\"" + server.arg(i) + "\",";
    }
  }
  message += "}}";
  server.send(404, "application/json", message);
  digitalWrite(LED_BUILTIN, HIGH); // Turn LED off
}
