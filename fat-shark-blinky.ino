#include <Event.h>
#include <Timer.h>
#include <Adafruit_NeoPixel.h>

#define MODE_FIRST          1
#define MODE_LIGHT          1
#define MODE_BLINK          2
#define MODE_PULSE          3
#define MODE_KNIGHT_RIDER   4
#define MODE_LAST           MODE_KNIGHT_RIDER

#define PIN_UP       1
#define PIN_DOWN     3
#define PIN_LEFT     4
#define PIN_RIGHT    5
#define PIN_MIDDLE   2
#define PIN_LED      0

#define BLACK (struct LED){0, 0, 0}
#define WHITE (struct LED){255, 255, 255}
#define RED (struct LED){255, 0, 0}
#define GREEN (struct LED){0, 255, 0}
#define BLUE (struct LED){0, 0, 255}
#define PINK (struct LED){255, 100, 100}
#define PURPLE (struct LED){255, 50, 255}
#define YELLOW (struct LED){255, 200, 0}
#define AQUA (struct LED){20, 255, 200}


struct KEY {
  char up = 0;
  bool fup = false;
  char down = 0;
  bool fdown = false;
  char left = 0;
  bool fleft = false;
  char right = 0;
  bool fright = false;
  char middle = 0;
  bool fmiddle = false;
};

struct LED {
  byte r;
  byte g;
  byte b;
};

struct LINE {
  LED led[7];
  byte level = 3;
  float power = 0.2;
  byte mode = MODE_LIGHT;
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIN_LED, NEO_GRB + NEO_KHZ800);
Timer timer;
KEY key;
LINE line;
LED lastColor;

bool change = true;
uint8_t c = 0;
uint16_t modeCounter = 0;
float rad = 0;
float amplitude = 0.1;

void setup() {

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_MIDDLE, INPUT_PULLUP);

  strip.begin();
  setColorAll(AQUA);
  lastColor = AQUA;
  line.mode = MODE_LIGHT;

  timer.every(10, mainLoop);
  timer.every(10, keyLoop);

}

void loop() {

  timer.update();

}


void keyLoop() {

  (digitalRead(PIN_UP) == LOW ? key.up++ : key.up = 0);
  (digitalRead(PIN_DOWN) == LOW ? key.down++ : key.down = 0);
  //  (digitalRead(PIN_LEFT) == LOW ? key.left++ : key.left = 0);
  //  (digitalRead(PIN_RIGHT) == LOW ? key.right++ : key.right = 0);
  (digitalRead(PIN_MIDDLE) == LOW ? key.middle++ : key.middle = 0);


  if (key.up > 10) {

    if (key.fup == false) {
      key.fup = true;
      change = true;
      nextMode();
    }
  }
  else {
    key.fup = false;
  }

  if (key.down > 10) {

    if (key.fdown == false) {
      key.fdown = true;
      nextColor();
    }
  }
  else {
    key.fdown = false;
  }

  if (key.middle > 10) {

    if (key.fmiddle == false) {
      key.fmiddle = true;
      nextPower();

    }
  }
  else {
    key.fmiddle = false;
  }

}

void nextPower() {
  line.level++;
  if (line.level > 3)
    line.level = 0;

  switch (line.level) {
    case 0:
      line.power = 0;
      amplitude = 0;
      strip.clear();
      strip.show();
      break;

    case 1:
      line.power = 0.1;
      amplitude = 0.08;
      setColorAll(lastColor.r * 0.1, lastColor.g * 0.1, lastColor.b * 0.1);
      break;

    case 2:
      line.power = 0.3;
      amplitude = 0.15;
      setColorAll(lastColor.r * 0.3, lastColor.g * 0.3, lastColor.b * 0.3);
      break;

    case 3:
      line.power = 1;
      amplitude = 0.5;
      setColorAll(lastColor);
      break;
  }
}

void setPower(float p) {

  p = max(min(p, 1), 0);
  line.level = p;
  setColorAll(lastColor.r * p, lastColor.g * p, lastColor.b * p);

}

void nextMode() {

  line.mode++;
  if (line.mode > MODE_LAST) {
    line.mode = MODE_FIRST;
  }
  modeCounter = 0;
}


void nextColor() {

  c++;
  if (c > 7)
    c = 0;

  switch (c) {
    case 1:
      setColorAll(RED);
      lastColor = RED;

      break;

    case 2:
      setColorAll(GREEN);
      lastColor = GREEN;

      break;

    case 3:
      setColorAll(BLUE);
      lastColor = BLUE;
      break;

    case 4:
      setColorAll(WHITE);
      lastColor = WHITE;
      break;

    case 5:
      setColorAll(PINK);
      lastColor = PINK;
      break;

    case 6:
      setColorAll(PURPLE);
      lastColor = PURPLE;
      break;
    case 7:
      setColorAll(AQUA);
      lastColor = AQUA;
      break;

    case 0:
      setColorAll(YELLOW);
      lastColor = YELLOW;
      break;

  }
}

void mainLoop() {
  modeCounter++;

  if (change) {

    switch (line.mode) {

      case MODE_LIGHT:
        setColorAll(lastColor);
        change = false;
        break;

      case MODE_BLINK:

        if (modeCounter == 12) {
          strip.clear();
          strip.show();
        }
        else if (modeCounter == 20) {
          modeCounter = 0;
          setColorAll(lastColor);
        }
        break;

      case MODE_PULSE:
        rad += 0.02;
        setPower(line.power + amplitude * sin(rad));
        break;

      case MODE_KNIGHT_RIDER:
        kitLight(modeCounter);
        break;
    }

  }
}

void kitLight(uint16_t counter) {

  uint8_t thing = counter / 10;
  if (thing > 20) {
    modeCounter = 0;
    thing = 0;
  }

  strip.clear();
  setKit(thing);
  strip.show();

}

void setKit(int8_t start) {
  //strip.setPixelColor(start - 2, lastColor.r * 0.05, lastColor.g * 0.05, lastColor.b * 0.05);
  strip.setPixelColor(start - 1, lastColor.r * 0.1, lastColor.g * 0.1, lastColor.b * 0.1);
  strip.setPixelColor(start, lastColor.r * 0.3, lastColor.g * 0.3, lastColor.b * 0.3);

  if (start > 9) {
    start = 10 + (10 - start);
 //   strip.setPixelColor(start + 2, lastColor.r * 0.05, lastColor.g * 0.05, lastColor.b * 0.05);
    strip.setPixelColor(start + 1, lastColor.r * 0.1, lastColor.g * 0.1, lastColor.b * 0.1);
    strip.setPixelColor(start, lastColor.r * 0.3, lastColor.g * 0.3, lastColor.b * 0.3);
  }
}

void setColorAll(uint8_t r, uint8_t g, uint8_t b) {
  for (uint8_t i = 0; i < 7; i++) {
    line.led[i].r = r;
    line.led[i].g = b;
    line.led[i].b = b;
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}

void setColorAll(LED led) {

  for (uint8_t i = 0; i < 7; i++) {
    line.led[i] = led;
    strip.setPixelColor(i, led.r * line.power, led.g * line.power, led.b * line.power);
  }
  strip.show();
}

