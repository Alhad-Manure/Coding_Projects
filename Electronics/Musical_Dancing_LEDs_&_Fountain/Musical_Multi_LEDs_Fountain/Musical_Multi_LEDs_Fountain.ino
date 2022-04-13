/******************************
Alhad Manure
Date: 09/09/2021
******************************/

#include "FastLED.h"
FASTLED_USING_NAMESPACE

#define NUM_LEDS 250
#define PIN 5
#define FRAMES_PER_SECOND  120

//Set Strip Constants
const uint16_t length = NUM_LEDS;
const uint16_t half = length/2;

CRGB leds[NUM_LEDS];

const byte L=A0;
const byte brightness = A4;

const byte pump1A=6;
const byte pump1B=7;
const byte pump1PWM=4;
const byte rlyPin=24;

const byte LEDMode=3;
const byte statusLED=13;

volatile uint8_t no = 1;

//Set up arrays for cycling through all the pixels.  I'm assuming you have an even number of lights.
uint32_t val_array[half][3];
uint32_t red_val, green_val, blue_val;
uint16_t l_val;
uint8_t time;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void setup()
{
 FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

 pinMode(L,INPUT);
 pinMode(brightness,INPUT);

 pinMode(LEDMode, INPUT_PULLUP);
 attachInterrupt(digitalPinToInterrupt(LEDMode), modeSelect, FALLING);
 
 pinMode(statusLED, OUTPUT);

 pinMode(pump1A, OUTPUT);
 pinMode(pump1B, OUTPUT);
 pinMode(pump1PWM, OUTPUT);
 pinMode(rlyPin, OUTPUT);

 digitalWrite(pump1A, LOW);
 digitalWrite(pump1B, HIGH);
 digitalWrite(rlyPin, HIGH); 
}

void loop()
{  
 l_val=analogRead(L);

 uint8_t pump1Speed = map(l_val, 0, 1023, 0, 255);

 if(pump1Speed < 65)
 {
  analogWrite(pump1PWM,70);
  digitalWrite(rlyPin, HIGH);
 }
 else
 {
  analogWrite(pump1PWM,200);
  digitalWrite(rlyPin, LOW);
 }
 
 pattern_select(no, l_val);
}

// *** Sets***
void set1(uint16_t val)
{ 
 if(val>560)
 { 
  Strobe(35, 17, 22, 5, 50, 0);//Pink
 }
 else if(val>=120 && val<=560)
 {
  Fire(55,120,5);
 }
 else if(val>70 && val<120)
 {
  Twinkle(35, 17, 22, 40, 100, false);//Pink
 } 
}

void set2(uint16_t val)
{ 
 if(val>560)
 {
  Strobe(49, 18, 12, 5, 50, 0);//Pink
 }
 else if(val>=120 && val<=560)
 {
  RunningLights(49, 18, 12, 5);
 }
 else if(val>70 && val<120)
 {
  NewKITT(49, 18, 12, 16, 10, 0);//Pink

 } 
}

void set3(uint16_t val)
{
  uint8_t maxBright = map(analogRead(brightness),0,1023,0,255);
  //Set the hue (0-255) and 24-bit color depending on left channel value
  byte hue_val = constrain(map(val, 0, 700, 0, maxBright), 0, maxBright);
  if(hue_val < 85)
  {
   red_val=hue_val * 3;
   green_val=0;
   blue_val=maxBright - hue_val * 3;;
   time=60;
  } 
  else if(hue_val < 170) 
  {
   hue_val -= 85;
   red_val=maxBright - hue_val * 3;
   green_val=hue_val * 3;
   blue_val=0;
   time=30;
  }
  else 
  {
   hue_val -= 170;
   red_val=0;
   green_val=maxBright - hue_val * 3;
   blue_val=hue_val * 3;
   time=0;
  }
    
  //Shift the current values.
  for (int i = 0; i<half-1; i++)
  {
    for(int j=0;j<3;j++)
    {
     val_array[i][j] = val_array[i+1][j];
    }
  }
  
  //Fill in the new value at the end of each array 
  val_array[half-1][0] = red_val;
  val_array[half-1][1] = green_val;
  val_array[half-1][2] = blue_val;
  
  //Go through each Pixel on the strip and set its color
  for (int i=0; i<half; i++)
  {
    //set pixel color
    //strip.setPixelColor(i, val_array[i]);
    setPixel(i, val_array[i][0], val_array[i][1], val_array[i][2]);
    //strip.setPixelColor(length-i-1, val_array[i]);
    setPixel(length-i-1, val_array[i][0], val_array[i][1], val_array[i][2]);
  }
  //Display the new values
  showStrip();
  //sample delay
  delay(time);  
}

void set4()
{
    uint8_t r,g,b;
    uint8_t maxBright = map(analogRead(brightness),0,1023,0,255);
    
    r=random(0,maxBright);
    g=random(0,maxBright);
    b=random(0,maxBright);
    for (int i=0; i<half; i++)
    {
     //set pixel color
     //strip.setPixelColor(i, r, g, b);
     setPixel(i, r, g, b);
     //strip.setPixelColor(length-i-1, r, g, b);
     setPixel(length-i-1, r, g, b);
     showStrip();
     delay(10);
    }
    delay(500);
    r=random(0,maxBright);
    g=random(0,maxBright);
    b=random(0,maxBright);
    for (int i=half-1; i>=0; i--)
    {
     //set pixel color
     //strip.setPixelColor(i, val_array[i]);
     setPixel(i, r, g, b);
     //strip.setPixelColor(length-i-1, r, g, b);
     setPixel(length-i-1, r, g, b);
     showStrip();
     delay(10);    
    }
    delay(500);
}

void pattern_select(char num,uint16_t val)
{
 switch(num)
 {
  case 1:
  {
    set1(val);
    break;
  }
  case 2:
  {
   set2(val);
   break;
  }
  case 3:
  {
    set3(val);
    break;
  }
  case 4:
  {
    set4();
    break;
  } 
 }
}


// *** Effects ***
void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause)
{
 for(int j = 0; j < StrobeCount; j++) 
 {
  setAll(red,green,blue);
  showStrip();
  delay(FlashDelay);
  setAll(0,0,0);
  showStrip();
  delay(FlashDelay);
 }
 delay(EndPause);
}

void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) 
{
 setAll(0,0,0);
 for (int i=0; i<Count; i++) 
 {
  setPixel(random(NUM_LEDS),red,green,blue);
  showStrip();
  delay(SpeedDelay);
  if(OnlyOne) 
  { 
   setAll(0,0,0); 
  }
 }
 delay(SpeedDelay);
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[half];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < half; i++) {
    cooldown = random(0, ((Cooling * 10) / half) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= half - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(0,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < half; j++) 
  {
   setPixelHeatColor(j, heat[j] );//length-i-1
   setPixelHeatColor(length-j-1, heat[j] );
  }

  for( int j = 0; j < half; j++) 
  {
   setPixelHeatColor(half+j-1, heat[j] );//length-i-1
   setPixelHeatColor(half-j-1, heat[j] );
  }
  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature)//Pink
{
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*49);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 49, heatramp, 12);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, heatramp, 0, 12);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, heatramp);
  }
}

// ***Set2***
// ***NewKITT***
void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) 
{
  for(int i =((NUM_LEDS-EyeSize)/2); i>0; i--) 
  {
    setAll(0,0,0);
    
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) 
    {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) 
    {
      setPixel(NUM_LEDS-i-j, red, green, blue); 
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);
    
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// *** RunningLights ***
void RunningLights(byte red, byte green, byte blue, int WaveDelay) 
{
  int Position=0;
  
  for(int i=0; i<3; i++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=(NUM_LEDS/2) - 1; i>=0; i--) 
      {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);        
      }
      for(int i=(NUM_LEDS/2) -1; i<NUM_LEDS; i++) 
      {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        setPixel(i,((sin(i-Position) * 127 + 128)/255)*red,
                   ((sin(i-Position) * 127 + 128)/255)*green,
                   ((sin(i-Position) * 127 + 128)/255)*blue);        
      }
      showStrip();
      delay(WaveDelay);
  }
}

// *** Common Methods ***

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
 #endif
}

void setAll(byte red, byte green, byte blue) 
{
  for(int i = 0; i < NUM_LEDS; i++ ) 
  {
    setPixel(i, red, green, blue); 
  }
  showStrip();
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
  delay(1000);
  digitalWrite(statusLED, LOW);
}
