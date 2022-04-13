/******************************
Alhad Manure
Date: 09/09/2021
******************************/

#include "FastLED.h"
#include "reactive_common.h"
FASTLED_USING_NAMESPACE

#define FASTLED_INTERRUPT_RETRY_COUNT 0 
#define FASTLED_ALLOW_INTERRUPTS 0
 
#define NUM_LEDS 46
#define PIN 5
#define FRAMES_PER_SECOND  120

#define MIC_LOW 0
#define MIC_HIGH 644

#define SAMPLE_SIZE 20
#define LONG_TERM_SAMPLES 250
#define BUFFER_DEVIATION 400
#define BUFFER_SIZE 3

//Set Strip Constants
const int length = NUM_LEDS*2;
const uint16_t half = length/2;

const byte L=A0;
const byte brightness = A4;

const byte pump1A=6;
const byte pump1B=7;
const byte pump1PWM=4;

const byte LEDMode=3;
const byte statusLED=13;

CRGB leds[2*length];

volatile uint8_t no = 1;
volatile bool constHeadFlg = false;

struct averageCounter *samples;
struct averageCounter *longTermSamples;
struct averageCounter *sanityBuffer;

float globalHue;
float globalBrightness = 255;
int hueOffset = 120;
float fadeScale = 1.3;
float hueIncrement = 0.7;

bool fade = false;

void setup()
{
 globalHue = 0;
 samples = new averageCounter(SAMPLE_SIZE);
 longTermSamples = new averageCounter(LONG_TERM_SAMPLES);
 sanityBuffer    = new averageCounter(BUFFER_SIZE);
  
 while(sanityBuffer->setSample(250) == true) {}
 while (longTermSamples->setSample(200) == true) {}
 
 //Serial.begin(38400);
 
 pinMode(L,INPUT);
 pinMode(brightness,INPUT);
 FastLED.addLeds<WS2811, PIN, GRB>(leds, 2*length).setCorrection( TypicalLEDStrip );

 pinMode(pump1A, OUTPUT);
 pinMode(pump1B, OUTPUT);
 pinMode(pump1PWM, OUTPUT);

 pinMode(LEDMode, INPUT_PULLUP);
 attachInterrupt(digitalPinToInterrupt(LEDMode), modeSelect, FALLING);
 
 pinMode(statusLED, OUTPUT);

 digitalWrite(pump1A, LOW);
 digitalWrite(pump1B, HIGH);

 digitalWrite(statusLED, LOW);
  
}

void loop()
{    
 int analogRaw = analogRead(L);
 uint8_t val = map(analogRead(brightness),0,1023,0,255);
 
 uint8_t pump1Speed = map(analogRaw, 0, 1023, 0, 255);
 if(no<4)
 {
   if(pump1Speed < 65)
   {
     analogWrite(pump1PWM,70);
   }
   else
   {
     analogWrite(pump1PWM,120);
   }
 }
 else
 {
    analogWrite(pump1PWM,190);
 }  
   
 patterns(no, analogRaw, val);
}

void patterns(char no, int analogRaw, uint8_t value )
{
 switch (no) 
 {
   case 1:
     fade = false;
     soundReactive1(analogRaw, value);
     break;
   case 2:
     chillFade();
     break;
   case 3:
     test(value);
     break;  
   default:
     clearAll();  
 }
}

// ********** Effects **********
void soundReactive1(int analogRaw, uint8_t bright) 
{
 int sanityValue = sanityBuffer->computeAverage();
 if (!(abs(analogRaw - sanityValue) > BUFFER_DEVIATION)) 
 {
    sanityBuffer->setSample(analogRaw);
 }
 analogRaw = fscale(MIC_LOW, MIC_HIGH, MIC_LOW, MIC_HIGH, analogRaw, 0.4);

 if (samples->setSample(analogRaw))
 return;
    
 uint16_t longTermAverage = longTermSamples->computeAverage();
 uint16_t useVal = samples->computeAverage();
 longTermSamples->setSample(useVal);

 int diff = (useVal - longTermAverage);
 if (diff > 5)
 {
   if (globalHue < 235)
   {
     globalHue += hueIncrement;
   }
 }
 else if (diff < -5)
 {
   if (globalHue > 2)
   {
     globalHue -= hueIncrement;
   }
 }
  
 int curshow = fscale(MIC_LOW, MIC_HIGH, 0.0, (float)NUM_LEDS, (float)useVal, 0);
 //int curshow = map(useVal, MIC_LOW, MIC_HIGH, 0, NUM_LEDS)

 for (int i = 0; i < NUM_LEDS; i++)
 {
   if ((NUM_LEDS-i) < curshow)
   {
     leds[i] = CHSV(globalHue + hueOffset + (i * 2), 255, bright);
     leds[length+i] = CHSV(globalHue + hueOffset + (i * 2), 255, bright);
     leds[length-i-1] = CHSV(globalHue + hueOffset + (i * 2), 255, bright);
     leds[(2*length)-i-1] = CHSV(globalHue + hueOffset + (i * 2), 255, bright);          
   }
   else
   {       
     leds[i] = CRGB(constrain(leds[i].r / fadeScale, 0, bright), constrain(leds[i].g / fadeScale, 0, bright), constrain(leds[i].b / fadeScale, 0, bright));
     leds[length+i] = CRGB(constrain(leds[i].r / fadeScale, 0, bright), constrain(leds[i].g / fadeScale, 0, bright), constrain(leds[i].b / fadeScale, 0, bright));
     leds[length-i-1] = CRGB(constrain(leds[i].r / fadeScale, 0, bright), constrain(leds[i].g / fadeScale, 0, bright), constrain(leds[i].b / fadeScale, 0, bright));
     leds[(2*length)-i-1] = CRGB(constrain(leds[i].r / fadeScale, 0, bright), constrain(leds[i].g / fadeScale, 0, bright), constrain(leds[i].b / fadeScale, 0, bright));     
   }   
 }
 //delay(2);
 FastLED.show(); 
}

void chillFade() 
{
  static int fadeVal = 0;
  static int counter = 0;
  static int from[3] = {0, 234, 255};
  static int to[3]   = {255, 0, 214};
  static int i, j;
  static double dsteps = 500.0;
  static double s1, s2, s3, tmp1, tmp2, tmp3;
  static bool reverse = false;
  if (fade == false) 
  {
    for (int i = 0; i < 2*length; i++) 
    {
      leds[i] = CRGB(from[0], from[1], from[2]);
    }
    s1 = double((to[0] - from[0])) / dsteps; 
    s2 = double((to[1] - from[1])) / dsteps; 
    s3 = double((to[2] - from[2])) / dsteps; 
    tmp1 = from[0], tmp2 = from[1], tmp3 = from[2];
    fade = true;
  }

  if (!reverse) 
  {
    tmp1 += s1;
    tmp2 += s2; 
    tmp3 += s3; 
  }
  else 
  {
    tmp1 -= s1;
    tmp2 -= s2; 
    tmp3 -= s3; 
  }

  for (j = 0; j < 2*length; j++)
  leds[j] = CRGB((int)round(tmp1), (int)round(tmp2), (int)round(tmp3)); 
  FastLED.show(); 
  delay(5);

  counter++;
  if (counter == (int)dsteps) 
  {
    reverse = !reverse;
    tmp1 = to[0], tmp2 = to[1], tmp3 = to[2];
    counter = 0;
  }
}

void test(int bright)
{
  int r,g,b;
  r=random(0,bright);
  g=random(0,bright);
  b=random(0,bright);
  
  for (int i=0; i<length; i++)
  {
   setPixel(i, r, g, b);
   setPixel((2*length)-i-1, r, g, b);
   showStrip();
  }

  r=random(0,bright);
  g=random(0,bright);
  b=random(0,bright);
  
  for (int i=length-1; i>=0; i--)
  {
   setPixel(i, r, g, b);
   setPixel((2*length)-i-1, r, g, b);
   showStrip();
  }     
}

float fscale(float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve)
{
  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;

  // condition curve parameter
  // limit range
  if (curve > 10)
  {
    curve = 10;
  }
  if (curve < -10)
  {
    curve = -10;
  }

  curve = (curve * -.1);  // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  // Check for out of range inputValues
  if (inputValue < originalMin)
  {
    inputValue = originalMin;
  }
  if (inputValue > originalMax)
  {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin)
  {
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal = zeroRefCurVal / OriginalRange; // normalize to 0 - 1 float

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax)
  {
    return 0;
  }

  if (invFlag == 0)
  {
    rangedValue = (pow(normalizedCurVal, curve) * NewRange) + newBegin;
  }
  else // invert the ranges
  {
    rangedValue = newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}

// ***Common Functions***
void clearAll() 
{
  for(int i = 0; i < 2*length; i++ ) 
  {
    setPixel(i, 0, 0, 0); 
  }
  showStrip();
}

void setPixel(int Pixel, byte red, byte green, byte blue) 
{
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
   delay(10);
 #endif
}

void showStrip() 
{
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

// ***ISR***
void modeSelect() 
{
  no++;
  
  if(no>4)
  {
    no=1; 
  }
   
  digitalWrite(statusLED, HIGH);
  digitalWrite(statusLED, LOW);
}
