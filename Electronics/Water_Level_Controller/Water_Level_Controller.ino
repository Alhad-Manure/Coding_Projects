/*************************
  Alhad Manure
  Date: 23/12/2021
*************************/
#include "EasyNextionLibrary.h"  // Include EasyNextionLibrary 

EasyNex myNex(Serial2); // Create an object of EasyNex class with the name < myNex >
                       // Set as parameter the Hardware Serial you are going to use.
                       
// defines pins numbers
const int trigPin = 9;
const int echoPin = 10;
const int valvRel=7;
const int mot=13;

void setup()
{
  Serial.begin(38400); // For debug messages
  myNex.begin(9600); // Begin the object with a baud rate of 9600
                     // If no parameter was given in the begin(), the default baud rate of 9600 will be used
  delay(500);        // Wait for Nextion to start

  pinMode(valvRel, OUTPUT);
  pinMode(mot, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  //myNex.writeNum("t0.bco=", 1024);
  //myNex.writeStr("t0.bco=", "2555");
  set_disp_para("t0.bco=", 2555);
  myNex.writeStr("t0.txt", "UND");
}

void loop() 
{
  int lev;
  int  motStat;

  lev=wat_level();
  Serial.print("Level: ");
  Serial.println(lev);

  myNex.NextionListen(); // This function must be called repeatedly to response touch events
                         // from Nextion touch panel. Actually, you should place it in your loop function.
  if(lev<=20)
  {
    set_disp_para("t0.bco=", 63488);
    set_txt("t0.txt=", "OFF");
    digitalWrite(valvRel,HIGH);
  }
  else
  {
    set_disp_para("t0.bco=", 1024);
    set_txt("t0.txt=", "ON");
    digitalWrite(valvRel,LOW);
  }
  lev=map(lev,20,70,0,100);
  Serial.print("Mapped level: ");
  Serial.println(lev);
  set_disp_para("j0.val=", 100-lev);
}

uint16_t wat_level()
{
  long duration;
  uint16_t distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  return distance;
}
void end_string()
{
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}

void set_disp_para(String parName, uint16_t parVal)
{
  Serial2.print(parName);
  Serial2.print(parVal);
  end_string();
}

void set_txt(String parName, String str)
{
  Serial2.print(parName);
  Serial2.print("\"");
  Serial2.print(str);
  Serial2.print("\"");
  end_string();
}

void trigger0()
{
  // To call this void send from Nextion's component's Event:  printh 23 02 54 00
  // In this exmaple, we send this command from the Release Event of b0 button (see the HMI of this example)
  // You can send  the same `printh` command, to call the same function, from more than one component, depending on your needs
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // If LED_BUILTIN is ON, turn it OFF, or the opposite
  if(digitalRead(LED_BUILTIN) == HIGH)
  {
    myNex.writeNum("b0.bco", 1024); // Set button b0 background color to GREEN (color code: 2016)
  }
  else if(digitalRead(LED_BUILTIN) == LOW)
  {
    myNex.writeNum("ba0.bco", 63488); // Set button b0 background color to RED (color code: 63488)
  }
}

