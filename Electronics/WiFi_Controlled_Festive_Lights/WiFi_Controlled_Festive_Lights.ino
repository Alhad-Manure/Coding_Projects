/*********
  Alhad Manure
  Date: 12/11/2021
*********/

// Load Wi-Fi library
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

// Replace with your network credentials
const char* ssid     = "**********";
const char* password = "**********";

// Set web server port number to 80
WiFiServer server(80);

// Neopixel Config
#define NeoPIN 15
#define NUM_LEDS 600
int brightness = 150;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NeoPIN, NEO_RGB + NEO_KHZ800);

const int led = 33;

// Decode HTTP GET value
String redString = "0";
String greenString = "0";
String blueString = "0";
String effectString = "0";
int redG, greenG, blueG;
int effectG;
int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;
int pos5 = 0;
int pos6 = 0;

// Variable to store the HTTP req  uest
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() 
{
  Serial.begin(115200);

  // ##############
  // NeoPixel start
  Serial.println();
  strip.setBrightness(brightness);
  strip.begin();
  strip.show(); 
  delay(50);
  Serial.println("NeoPixel started");
  
  // Connect to Wi-Fi network with SSID and password
  // #########
  // Webserver
  pinMode ( led, OUTPUT );
  digitalWrite ( led, 0 );
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop()
{
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) // If a new client connects,
  {                             
    currentTime = millis();
    previousTime = currentTime;
    //Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) // loop while the client's connected 
    {            
      currentTime = millis();
      if (client.available()) // if there's bytes to read from the client,
      {             
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') // if the byte is a newline character
        {                    
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) 
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
                   
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<style> body{background-image: url('https://timesofindia.indiatimes.com/thumb/msid-79184301,width-1200,height-900,resizemode-4/.jpg');background-repeat:no-repeat;background-attachment:fixed;background-size:100% 100%;}");
            client.println(".backColour {background-color:white;}");
            client.println(".center {text-align: center; color:white;}</style>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">");
            client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>");
            client.println("</head><body><div class=\"container\"><div class=\"center\"><h1>!!!Happy Diwali!!!</h1></div>");
            client.println("<p><label class=\"backColour\" for=\"rgb\">Choose Colour:</label>");
            client.println("<input class=\"jscolor {onFineChange:'update(this)'}\" id=\"rgb\">");
            client.println("<a class=\"btn btn-primary btn-lg\" href=\"#\" id=\"change_color\" role=\"button\">Change Color</a></p></div>");
            client.println("<script>function update(picker) {document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' +  Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);");
            client.println("document.getElementById(\"change_color\").href=\"?r\" + Math.round(picker.rgb[0]) + \"g\" +  Math.round(picker.rgb[1]) + \"b\" + Math.round(picker.rgb[2]) + \"&\";}</script>");
            client.println("<div class=\"container\"><p><form><label class=\"backColour\" for=\"change_effect\">Choose Effect:</label>");
            client.println("<select id=\"effects\" onchange=\"myFunction()\">");
            client.println("<option value=\"1\">-Select-</option>");
            client.println("<option value=\"2\">Bouncing Balls</option>");
            client.println("<option value=\"3\">Running Lights</option>");
            client.println("<option value=\"4\">Meteor Rain</option>");
            client.println("<option value=\"5\">Sparkle</option>");
            client.println("<option value=\"6\">Multicolour Swipe</option>");
            client.println("<option value=\"7\">NewKITT</option>");
            client.println("<option value=\"8\">Rainbow Cycle</option>");
            client.println("<option value=\"9\">Fire</option>");
            client.println("<option value=\"10\">Two Side Swipe</option>");
            client.println("<option value=\"11\">Double Swipe</option>");
            client.println("<option value=\"12\">Plane Colour</option></select>");
            client.println("<a class=\"btn btn-primary btn-lg\" href=\"#\" id=\"change_effect\" role=\"button\">Change Effect</a></form></p></div>");
            client.println("<script>function myFunction(){var x = document.getElementById('effects').value;");
            client.println("document.getElementById(\"change_effect\").href=\"?e\" + x + \"$\";}</script></body></html>");
            // The HTTP response ends with another blank line
            client.println();

            // Request sample: /?r201g32b255&
            // Red = 201 | Green = 32 | Blue = 255
            if(header.indexOf("GET /?r") >= 0) 
            {
              pos1 = header.indexOf('r');
              pos2 = header.indexOf('g');
              pos3 = header.indexOf('b');
              pos4 = header.indexOf('&');
              redString = header.substring(pos1+1, pos2);
              greenString = header.substring(pos2+1, pos3);
              blueString = header.substring(pos3+1, pos4);
              
              // splitting into three parts
              redG = redString.toInt();
              greenG = greenString.toInt();
              blueG = blueString.toInt();             
            }
            if(header.indexOf("GET /?e") >= 0)
            {
              pos5 = header.indexOf('e');
              pos6 = header.indexOf('$');
              effectString = header.substring(pos5+1, pos6);
              effectG = effectString.toInt();
            }
            // Break out of the while loop
            break;
          } 
          else // if you got a newline, then clear currentLine 
          { 
            currentLine = "";
          }
        } 
        else if (c != '\r') // if you got anything else but a carriage return character, 
        {  
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
  selectEffect();
}

void selectEffect()
{
  switch(effectG)
  { 
    case 1:
    {
      //This case will never arise.
      break;  
    }
    case 2:
    {
      //BouncingBalls(byte red, byte green, byte blue, int BallCount)
      BouncingBalls(greenG, redG, blueG, 400);
      break;
    }
    case 3:
    {
      //RunningLights(int red, int green, int blue, int WaveDelay)
      RunningLights(greenG, redG, blueG, 12);
      break;
    }
    case 4:
    {
      //meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
      meteorRain(greenG, redG, blueG, 200, 64, true, 2);
      break;
    }
    case 5:
    {
      //Sparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay)
      Sparkle(greenG, redG, blueG, 5, random(3,10));
      break;
    }
    case 6:
    {
      //swipeMultiColour()
      swipeMultiColour();
      break;
    }
    case 7:
    {
      //NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay))
      NewKITT(greenG, redG, blueG,  120, 10, 50);
      break;
    }
    case 8:
    {
      //rainbowCycle(int SpeedDelay)
      rainbowCycle(20);
      break;
    }
    case 9:
    {
      //Fire(int Cooling, int Sparking, int SpeedDelay)
      Fire(35,200,15);
      break;
    }
    case 10:
    {
      //TwoSideSwipe(byte red, byte green, byte blue)
      TwoSideSwipe(greenG, redG, blueG);
      break;
    }
    case 11:
    {
      //TwoSideSwipe()
      TwoSideDoubleSwipe();
      break;
    }
    case 12:
    {
      //plane(int r, int g, int b)
      plane(greenG, redG, blueG);
      break;
    }
  }  
}

/*****************************
        RunningLights
*****************************/

void RunningLights(int red, int green, int blue, int WaveDelay) 
{
  int Position=0;
 
  for(int j=0; j<NUM_LEDS*2; j++)
  {
    Position++; // = 0; //Position + Rate;
    for(int i=0; i<NUM_LEDS; i++) 
    {
      // sine wave, 3 offset waves make a rainbow!
      //float level = sin(i+Position) * 127 + 128;
      //setPixel(i,level,0,0);
      //float level = sin(i+Position) * 127 + 128;
      setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                 ((sin(i+Position) * 127 + 128)/255)*green,
                 ((sin(i+Position) * 127 + 128)/255)*blue);
    } 
    showStrip();
    delay(WaveDelay);
  }
}

/*****************************
            plane
*****************************/

void plane(int r, int g, int b)
{
  // setting whole strip to the given color
  for(int i=0; i < NUM_LEDS; i++) 
  {
    setPixel(i, r, g, b);
  }
  // init
  showStrip();  
}

/*****************************
         BouncingBalls
*****************************/

void BouncingBalls(byte red, byte green, byte blue, int BallCount) 
{
  float Gravity = -9.81;
  int StartHeight = 1;
 
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
 
  for (int i = 0 ; i < BallCount ; i++) 
  {  
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
  }

  while (true) 
  {
    for (int i = 0 ; i < BallCount ; i++) 
    {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
 
      if ( Height[i] < 0 ) 
      {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
 
        if ( ImpactVelocity[i] < 0.01 ) 
        {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }
 
    for (int i = 0 ; i < BallCount ; i++) 
    {
      setPixel(Position[i],red,green,blue);
    }
   
    showStrip();
    setAll(0,0,0);
  }
}

/*****************************
         meteorRain
*****************************/

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) 
{  
  setAll(0,0,0);
 
  for(int i = 0; i < NUM_LEDS+NUM_LEDS; i++) 
  {
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) 
    {
      if( (!meteorRandomDecay) || (random(10)>5) ) 
      {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) 
    {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) 
      {
        setPixel(i-j, red, green, blue);
      }
    }
   
    showStrip();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue) 
{
  #ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
   
    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
   
    strip.setPixelColor(ledNo, r,g,b);
  #endif
  #ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    leds[ledNo].fadeToBlackBy( fadeValue );
  #endif  
}

/*****************************
           Sparkle
*****************************/

void Sparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) 
{
  setAll(red,green,blue);
 
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,0xff,0xff,0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
}

/*****************************
      rainbowCycle
*****************************/

void rainbowCycle(int SpeedDelay) 
{
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++)// 5 cycles of all colors on wheel 
  { 
    for(i=0; i< NUM_LEDS; i++) 
    {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    showStrip();
    delay(SpeedDelay);
  }
}

byte * Wheel(byte WheelPos) 
{
  static byte c[3];
 
  if(WheelPos < 85) 
  {
    c[0]=WheelPos * 3;
    c[1]=255 - WheelPos * 3;
    c[2]=0;
  } 
  else if(WheelPos < 170) 
  {
    WheelPos -= 85;
    c[0]=255 - WheelPos * 3;
    c[1]=0;
    c[2]=WheelPos * 3;
  } 
  else 
  {
    WheelPos -= 170;
    c[0]=0;
    c[1]=WheelPos * 3;
    c[2]=255 - WheelPos * 3;
  }

  return c;
}

/*****************************
      swipeMultiColour
*****************************/

void swipeMultiColour()
{
  uint16_t half = NUM_LEDS/2;
  int r,g,b;
  r=random(0,180);
  g=random(0,180);
  b=random(0,180);
  
  for (int i=0; i<half; i++)
  {
   setPixel(i, r, g, b);
   setPixel(NUM_LEDS-i-1, r, g, b);
   showStrip();
  }

  r=random(0,180);
  g=random(0,180);
  b=random(0,180);
  
  for (int i=half-1; i>=0; i--)
  {
   setPixel(i, r, g, b);
   setPixel(NUM_LEDS-i-1, r, g, b);
   showStrip();
  }     
}

/*****************************
          NewKITT
*****************************/

void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i =((NUM_LEDS-EyeSize)/2); i>=0; i--) {
    setAll(0,0,0);
   
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
   
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue);
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);
   
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i<=((NUM_LEDS-EyeSize)/2); i++) {
    setAll(0,0,0);
   
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
   
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue);
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);
   
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

/*****************************
            Fire
*****************************/

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, heatramp, 255, 0);
  } else {                               // coolest
    setPixel(Pixel, 0, heatramp, 0);
  }
}

/*****************************
         TwoSideSwipe
*****************************/
void TwoSideSwipe(byte red, byte green, byte blue) 
{ 
  //strip.setBrightness(150);
  for (int i = 0; i < NUM_LEDS / 2; i++) 
  {
    setPixel(i, red, green, blue);
    //leds[i] = CRGB(0, 0, 255);
    setPixel(600 - i, red, green, blue);
    //leds[600 - i] = CRGB(0, 0, 255);
    showStrip();
    delay(15);
  }
  //delay(1000);
  for (int i = 0; i < NUM_LEDS / 2; i++) 
  {
    setPixel(i, red, green, blue);
    //leds[i] = CRGB(0, 0, 0);
    setPixel(600 - i, red, green, blue);
    //leds[24 - i] = CRGB(0, 0, 0);
    showStrip();
    delay(15);
  }
}

/*****************************
       TwoSideDoubleSwipe
*****************************/
void TwoSideDoubleSwipe() 
{ 
  //strip.setBrightness(150);

  for (int i = NUM_LEDS; i >= 0; i--) 
  {
    setPixel(i, 255 - 10 * i, 0 + 10 * i, 0);
    //leds[i] = CRGB(255 - 10 * i, 0 + 10 * i, 0);
    showStrip();
    delay(2);
  }
  //delay(1000);
  for (int i = 0; i < NUM_LEDS; i++) 
  {
    setPixel(i, 0, 0, 0);
    showStrip();
    delay(2);
  }
  for (int i = 0; i < NUM_LEDS; i++) 
  {
    setPixel(i, 0, 0 + 10 * i, 255 - 10 * i);
    //leds[i] = CRGB(0, 0 + 10 * i, 255 - 10 * i);
    showStrip();
    delay(2);
  }
  //delay(1000);
  for (int i = NUM_LEDS; i >= 0; i--) 
  {
    setPixel(i, 0, 0, 0);
    showStrip();
    delay(2);
  }
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

