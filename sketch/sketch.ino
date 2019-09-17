#include <FastLED.h>

#define STRIP_PIN 2
#define STRIP_LENGTH 4
#define FPS 30
#define SPEED 1
#define BRIGHTNESS 128

class StrobeProvider {

  private:
    CHSV base;
    uint8_t distance;

  public:
    StrobeProvider() {
      this->base = CHSV(0, 0, 0);
      this->distance = 0;
    }
  
    CHSV getFirst() {
      return base;
    }
  
    CHSV getSecond() {
      uint16_t hue = base.h + distance;
      if (hue > 255) { 
        hue -= 255; 
      }
      return CHSV(hue, base.saturation, base.value);
    }
  
    void setBase(CHSV newBase) {
      base = newBase;
    }
  
    void setDistance(uint16_t newDistance) {
      distance = newDistance;
    }

    void shiftBase(int delta) {
      uint16_t newHue = base.hue + delta;
      if (newHue < 0) { 
        newHue += 255; 
      }
      else if (newHue > 255) { 
        newHue -= 255; 
      }
      base.hue = newHue;
    }

    void shiftDistance(int delta) {
      uint16_t newDistance = distance + delta;
      if (newDistance < 0) { 
        newDistance += 255; 
      }
      else if (newDistance > 255) { 
        newDistance -= 255; 
      }
      distance = newDistance;
    }
};

CRGB leds[STRIP_LENGTH];
StrobeProvider strobe = StrobeProvider();

void setup() {
  pinMode(STRIP_PIN, OUTPUT);
  FastLED.addLeds<NEOPIXEL, STRIP_PIN>(leds, STRIP_LENGTH);
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < STRIP_LENGTH; i++) {
    leds[i] = CRGB::Blue;
  }
  FastLED.show();
  strobe.setBase(CHSV(0 , 255, 255));
  strobe.setDistance(0);
}

unsigned long lastStrobe = millis();
bool isStrobed = false;
void loop() {
  if (millis() - lastStrobe > 60) {
    isStrobed = !isStrobed;
    CHSV colour = isStrobed ? strobe.getSecond() : strobe.getFirst();
    for (int i = 0; i < STRIP_LENGTH; i++) {
      leds[i] = colour;
    }
    FastLED.show();
    lastStrobe = millis();
    strobe.shiftDistance(2);
    strobe.shiftBase(1);
  }
}
