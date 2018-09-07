////////////////////////////////////////////////////////
// Vortex StrandTest - arkadiraf@gmail.com            //
// Strand test based on adafruit code - 11/04/2018    //
////////////////////////////////////////////////////////
///////////////
// Libraries //
///////////////
#include "Adafruit_NeoPixel.h"
#include <stdlib.h> // for the atoi() function
///////////////
// #defines  //
///////////////
#define DEBUG_MSG

///////////////
// Variables //
///////////////
// command time stamp
volatile unsigned int CMDTimeStamp = 0;
// command ready for leds update
volatile int CMD_Ready = 0;

int pump_1_pin = 10; //PWM
int pump_2_pin = 11; //PWM
int fan_pin = 9; //DIGITAL
int mist_pin = 8; //DIGITAL
int led_pin = 13; //DIGITAL

volatile int pump_1_pwm = 0;
volatile int pump_2_pwm = 0;
volatile int fan_state = 0;
volatile int mist_state = 0;
volatile int leds_mode = 0;
volatile int last_leds_mode = 0;
// neopixel LED Pin
#define STRIP_PIN A0

// neopixel LEDS
#define NUM_LEDS 190

uint8_t brightness = 50;
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

  // init pins
  pinMode(pump_1_pin, OUTPUT);
  pinMode(pump_2_pin, OUTPUT);
  pinMode(fan_pin, OUTPUT);
  pinMode(mist_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(STRIP_PIN, OUTPUT);

  // reset state
  analogWrite(pump_1_pin, pump_1_pwm);
  analogWrite(pump_2_pin, pump_2_pwm);
  digitalWrite(fan_pin, fan_state);
  digitalWrite(mist_pin, mist_state);
  digitalWrite(led_pin, HIGH);

  // init neopixels
  strip.begin();
  strip.setBrightness(brightness);
  strip.show(); // Initialize all pixels to 'off'
  colorWipe(strip.Color(0, 255, 0), 1);
}

void loop() {
  if ((CMD_Ready) && (last_leds_mode != leds_mode)) {
    CMD_Ready = 0;
    last_leds_mode = leds_mode;
#ifdef DEBUG_MSG
    // Print Command and Value
    Serial.print("Leds Mode: ");
    Serial.println(leds_mode);
#endif /* DEBUG_MSG */
    switch (leds_mode) {
      case 0:
        colorWipe(strip.Color(0, 0, 0), 1);
        break;
      case 1:
        rainbow(1);
        break;
      case 2:
        colorWipe(strip.Color(255, 0, 0), 1);
        break;
      case 3:
        colorWipe(strip.Color(0, 255, 0), 1);
        break;
      case 4:
        colorWipe(strip.Color(0, 0, 255), 1);
        break;
      default:
        colorWipe(strip.Color(0, 0, 0), 1);
        break;
    }
  } // end new leds command
  unsigned int timeout_cmd = (millis() - CMDTimeStamp);
  if (abs(timeout_cmd) > 5000) { // no command for the last 5 seconds basic automatic mode
    analogWrite(pump_1_pin, 255);
    analogWrite(pump_2_pin, 255);
    digitalWrite(mist_pin, LOW);
    digitalWrite(led_pin, HIGH);
    rainbow(3);
    analogWrite(pump_1_pin, 0);
    analogWrite(pump_2_pin, 0);
    digitalWrite(mist_pin, LOW);
    digitalWrite(led_pin, LOW);
    rainbow(2);
  }
} // end main

/////////////////
// SerialEvent //
/////////////////
/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    ParseString(inChar);
  }
} // end serial event


/////////////////////////////
// Parse Recieved Command  //
/////////////////////////////
// Incoming Message parser  Format: "$<CMD>,<Value>\r\n" // up to /r/n
void ParseString(char inbyte)
{
  static const uint16_t BufferCMDSize = 32;
  static const uint16_t BufferCMD_ValuesSize = 5;

  static int CMD_Values[BufferCMD_ValuesSize] = {0};
  static char BufferCMD[BufferCMDSize] = {0};
  static uint16_t BufferIndex = 0;
  static uint16_t Values_Index = 0;

  BufferIndex = BufferIndex % (BufferCMDSize); // simple overflow handler
  Values_Index = Values_Index % (BufferCMD_ValuesSize); // simple overflow handler

  BufferCMD[BufferIndex] = inbyte;
  BufferIndex++;

  // parse incoming message
  if (inbyte == '$') { // start of message
    BufferIndex = 0; // initialize to start of parser
    Values_Index = 0; // index for values position
  } else if (inbyte == ',') { // seperator char
    CMD_Values[Values_Index] = atoi(BufferCMD); // input value to buffer values
    BufferIndex = 0; // initialize to start of parser
    Values_Index++;
    if (Values_Index >= (BufferCMD_ValuesSize - 1)) Values_Index = (BufferCMD_ValuesSize - 1); // buffer overflow
  } else if (inbyte == '\r') { // end of message // parse message
    // Update last value
    CMD_Values[Values_Index] = atoi(BufferCMD); // input value to buffer values

    BufferIndex = 0; // initialize to start of parser
    Values_Index = 0; // reset values index

    // set time stamp commad
    CMDTimeStamp = millis();

    //////////////////////
    // Command recievd: //
    //////////////////////
    int value_1 = CMD_Values[0];
    int value_2 = CMD_Values[1];
    int value_3 = CMD_Values[2];
    int value_4 = CMD_Values[3];
    int value_5 = CMD_Values[4];

#ifdef DEBUG_MSG
    // Print Command and Value
    Serial.print(value_1);
    Serial.print(",");
    Serial.print(value_2);
    Serial.print(",");
    Serial.print(value_3);
    Serial.print(",");
    Serial.print(value_4);
    Serial.print(",");
    Serial.println(value_5);
#endif /* DEBUG_MSG */

    // apply commands
    pump_1_pwm = value_1;
    pump_2_pwm = value_2;
    fan_state = value_3;
    mist_state = value_4;
    leds_mode = value_5;

    // apply to pins
    analogWrite(pump_1_pin, pump_1_pwm);
    analogWrite(pump_2_pin, pump_2_pwm);
    digitalWrite(fan_pin, fan_state);
    digitalWrite(mist_pin, mist_state);
    CMD_Ready = 1;
  }//end parser
}// end parser function


/////////////////////////////////////////
// Adafruit NeoPixel Example functions //
/////////////////////////////////////////
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
