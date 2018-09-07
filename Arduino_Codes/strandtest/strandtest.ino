////////////////////////////////////////////////////////
// Vortex StrandTest - arkadiraf@gmail.com            //
// Strand test based on adafruit code - 11/04/2018    //
////////////////////////////////////////////////////////
///////////////
// Libraries //
///////////////
#include "Adafruit_NeoPixel.h"
///////////////
// Variables //
///////////////
int pump_1_pin = 10; //PWM
int pump_2_pin = 11; //PWM
int fan_pin = 9; //DIGITAL
int mist_pin = 8; //DIGITAL
int led_pin = 13; //DIGITAL

// neopixel LED Pin
#define STRIP_PIN A0

// neopixel LEDS
#define NUM_LEDS 190

uint8_t brightness=25;
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(57600); //This pipes to the serial monitor
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  pinMode(pump_1_pin,OUTPUT);
  pinMode(pump_2_pin,OUTPUT);
  pinMode(fan_pin,OUTPUT);
  pinMode(mist_pin,OUTPUT);
  pinMode(led_pin,OUTPUT);

  digitalWrite(fan_pin,LOW);
  digitalWrite(mist_pin,HIGH);
}

void loop() {
  static int Status=0;
  Serial.println("Hello world!!!!");  // Print "Hello World" to the Serial Monitor
  Status=!Status; 
  analogWrite(pump_1_pin,255);
  analogWrite(pump_2_pin,255);
  //digitalWrite(fan_pin,HIGH);
  //digitalWrite(mist_pin,HIGH);
  digitalWrite(led_pin,HIGH);
  rainbow(2);
  analogWrite(pump_1_pin,0);
  analogWrite(pump_2_pin,0);
  //digitalWrite(fan_pin,LOW);
  //digitalWrite(mist_pin,LOW);
  digitalWrite(led_pin,LOW);
  rainbow(2);
}

// Adafruit NeoPixel Example functions //
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
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
