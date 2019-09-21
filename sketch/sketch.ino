#include <FastLED.h>

#define STRIP_PIN 5
#define STRIP_LENGTH 4
#define INPUT_PIN A0
#define A_PIN 13
#define B_PIN 15
#define FPS 30
#define SPEED 1
#define BRIGHTNESS 64
#define JOY_DEAD_ZONE 10
#define MAX_STROBE_SPEED 30

class StrobeProvider {

  private:
    CHSV base;
    uint8_t distance;
    uint8_t speed; // strobes per second

  public:
    StrobeProvider() {
      this->base = CHSV(0, 0, 0);
      this->distance = 128;
      this->speed = 0;
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

    uint8_t getSpeed() {
      return speed;
    }
  
    void setBase(CHSV newBase) {
      base = newBase;
    }
  
    void setDistance(uint16_t newDistance) {
      distance = newDistance;
    }

    void setSpeed(uint8_t newSpeed) {
      speed = newSpeed;
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

void updateFromInputs() {
  digitalWrite(A_PIN, LOW);
  digitalWrite(B_PIN, LOW);
  int joyX = map(analogRead(INPUT_PIN), 18, 1024, -100, 100);
  digitalWrite(A_PIN, LOW);
  digitalWrite(B_PIN, HIGH);
  int joyY = map(analogRead(INPUT_PIN), 18, 1024, -100, 100);
  digitalWrite(A_PIN, HIGH);
  digitalWrite(B_PIN, LOW);
  int slider = map(analogRead(INPUT_PIN), 18, 1024, 0, 100);
  strobe.setSpeed(map(slider, 0, 100, 0, MAX_STROBE_SPEED));
  debugInputValues(joyX, joyY, strobe.getSpeed());
}

void debugInputValues(int joyX, int joyY, int slider) {
  Serial.print(joyX);
  Serial.print(" ");
  Serial.print(joyY);
  Serial.print(" ");
  Serial.println(slider);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Start debug");
  pinMode(STRIP_PIN, OUTPUT);
  pinMode(A_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  pinMode(INPUT_PIN, INPUT);
  digitalWrite(A_PIN, LOW);
  digitalWrite(B_PIN, LOW);
  FastLED.addLeds<NEOPIXEL, STRIP_PIN>(leds, STRIP_LENGTH);
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < STRIP_LENGTH; i++) {
    leds[i] = CRGB::Blue;
  }
  FastLED.show();
  strobe.setBase(CHSV(0 , 255, 255));
  strobe.setDistance(128);
}

unsigned long lastStrobe = millis();
bool isStrobed = false;
void loop() {
  updateFromInputs();
  if (strobe.getSpeed() > 0 && millis() - lastStrobe > 1000 / strobe.getSpeed()) {
    isStrobed = !isStrobed;
    CHSV colour = isStrobed ? strobe.getSecond() : strobe.getFirst();
    for (int i = 0; i < STRIP_LENGTH; i++) {
      leds[i] = colour;
    }
    FastLED.show();
    lastStrobe = millis();
  }
}
