/*
 * place arduino inside a lamp,
 * add a neopixel led strip
 * and a button
 * to cycle between some predefined led layouts
 */

#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN   2
#define PIXEL_PIN    3
#define PIXEL_COUNT 16
#define INTERRUPTDELAY 1000   // 1 sec
#define NUM_MODES 12

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ400);

char *MODES[] = { "Black/off", "Stroboscope", "colorWipe - White", "colorWipe - Red",
     "colorWipe - Green", "colorWipe - Blue", "theaterChase - White",
     "theaterChase - Red", "theaterChase - Green", "theaterChase - Blue",
     "rainbow", "rainbowCycle", "theaterChaseRainbow" };

int showMode = 0;
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
    Serial.print("Mode: "); Serial.println(MODES[showMode]);
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
    // colorWipe - White
    case 2:
            colorWipe(strip.Color(255, 255, 255), 10);
            break;
    // colorWipe - Red
    case 3:
            colorWipe(strip.Color(255, 0, 0), 10);
            break;
    // colorWipe - Green
    case 4:
            colorWipe(strip.Color(0, 255, 0), 10);
            break;
    // colorWipe - Blue
    case 5:
            colorWipe(strip.Color(0, 0, 255), 10);
            break;
    // theaterChase - White
    case 6:
            theaterChase(strip.Color(127, 127, 127), 50);
            break;
    // theaterChase - Red
    case 7:
            theaterChase(strip.Color(127,   0,   0), 50);
            break;
    // theaterChase - Green
    case 8:
            theaterChase(strip.Color(  0,   127, 0), 50);
            break;
    // theaterChase - Blue
    case 9:
            theaterChase(strip.Color(  0,   0, 127), 50);
            break;
    // rainbow
    case 10:
            rainbow(20);
            break;
    // rainbowCycle
    case 11:
            rainbowCycle(20);
            break;
    // theaterChaseRainbow
    case 12:
            theaterChaseRainbow(50);
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
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    if (oldMode != showMode) return;
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  int oldMode = showMode;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    if (oldMode != showMode) return;
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  int oldMode = showMode;

  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      if (oldMode != showMode) return;
      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  int oldMode = showMode;
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();
      if (oldMode != showMode) return;
      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
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
