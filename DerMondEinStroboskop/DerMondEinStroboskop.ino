/*
 * place arduino inside a lamp,
 * add a neopixel led strip
 * and a button
 * to cycle between some predefined led layouts
 */

#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN      2
#define PIXEL_PIN       3
#define PIXEL_COUNT     16
#define INTERRUPTDELAY  1000    // 1 sec
#define NUM_MODES       3       // ignore static colors for now

void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

char *MODES[] = { "Black/off", "Stroboscope", "rainbow", "saved",
     "colorWipe - White", "colorWipe - Red", "colorWipe - Green", "colorWipe - Blue",
     "colorWipe - Turquoise", "colorWipe - Purple", "colorWipe - Orange" };

volatile int showMode = 0;
volatile uint32_t color = 0;
unsigned long interruptTime;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  Serial.begin(115200);
  strip.show(); // Initialize all pixels to 'off'
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleInterrupt, CHANGE);
}

void handleInterrupt() {
  unsigned long currentTime = millis();
  if (currentTime - interruptTime > INTERRUPTDELAY) {
    interruptTime = millis();
    showMode++;
    if (showMode > NUM_MODES) showMode=0;
    Serial.print("Mode["); Serial.print(showMode);
    Serial.print("] "); Serial.println(MODES[showMode]);
  }
}

void loop() {
  startShow(showMode);
}

void startShow(int i) {
  switch(i){
    // Black/off
    case 0:
            colorWipe(strip.Color(0, 0, 0), 10);
            break;
    // Stroboscope
    case 1:
            colorWipe(strip.Color(0, 0, 0), 0);
            delay(25);
            colorWipe(strip.Color(255, 255, 255), 0);
            break;
    // rainbow
    case 2:
            rainbow(20);
            break;
    // saved
    case 3:
            colorWipe(color, 20);
            break;
    // colorWipe - White
    case 4:
            colorWipe(strip.Color(255, 255, 255), 10);
            break;
    // colorWipe - Red
    case 5:
            colorWipe(strip.Color(255, 0, 0), 10);
            break;
    // colorWipe - Green
    case 6:
            colorWipe(strip.Color(0, 255, 0), 10);
            break;
    // colorWipe - Blue
    case 7:
            colorWipe(strip.Color(0, 0, 255), 10);
            break;
    // colorWipe - turquoise
    case 8:
            colorWipe(strip.Color(38, 139, 210), 10);
            break;
    // colorWipe - purple
    case 9:
            colorWipe(strip.Color(255, 0, 127), 10);
            break;
    // colorWipe - orange
    case 10:
            Serial.println("orange");
            colorWipe(strip.Color(255, 102, 0), 10);
            break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  int oldMode = showMode;

  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    if (oldMode != showMode) return;
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  int oldMode = showMode;
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      color = Wheel((i+j) & 255);
      strip.setPixelColor(i, color);
    }
    strip.show();
    if (oldMode != showMode) return;
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
