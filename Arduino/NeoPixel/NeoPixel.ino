#include <FastLED.h>

// Define the number of LEDs and matrix dimensions
#define LED_PIN    5      // Pin connected to the NeoPixel data line
#define MATRIX_WIDTH  8   // Width of the matrix
#define MATRIX_HEIGHT 8   // Height of the matrix
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

// LED array
CRGB leds[NUM_LEDS];

// Helper function to map (x, y) coordinates to the LED index
int getIndex(int x, int y) {
  if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) return -1;
  return (y % 2 == 0) ? (y * MATRIX_WIDTH + x) : (y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x));
}

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);
  Serial.println("NeoPixel Matrix Example!");

  // Initialize the FastLED library
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
  FastLED.clear();
  FastLED.show();
}

void drawPixel(int x, int y, CRGB color) {
  int index = getIndex(x, y);
  if (index >= 0 && index < NUM_LEDS) {
    leds[index] = color;
  }
}

void drawCheckerboard() {
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      if ((x + y) % 2 == 0) {
        drawPixel(x, y, CRGB::Red);
      } else {
        drawPixel(x, y, CRGB::Black);
      }
    }
  }
  FastLED.show();
}

void drawScrollingRainbow() {
  static uint8_t hueOffset = 0;
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      int index = getIndex(x, y);
      leds[index] = CHSV((hueOffset + x * 10 + y * 10) % 255, 255, 255);
    }
  }
  hueOffset++;
  FastLED.show();
  delay(50);
}

void loop() {
  // Alternate between two animations
  static uint32_t lastSwitch = 0;
  static bool showCheckerboard = true;

  if (millis() - lastSwitch > 5000) {
    showCheckerboard = !showCheckerboard;
    lastSwitch = millis();
  }

  if (showCheckerboard) {
    drawCheckerboard();
    delay(500);
  } else {
    drawScrollingRainbow();
  }
}
