/*#include<SoftwareSerial.h>
const int RX=2;                              
const int TX=3;
SoftwareSerial mySerial(RX,TX);
*/
const int Right1=7;                              // Declaring Motor Driver pins
const int Right2=8;                              // Declaring Motor Driver pins
const int Left1=9;                             // Declaring Motor Driver pins
const int Left2=4;                             // Declaring Motor Driver pins
const int Right_PWM=5;                           // Declaring Motor Driver pins
const int Left_PWM=6;                          // Declaring Motor Driver pins

/*****Speed Variables*****/
uint8_t x,y;                                    // Variables to read Searial buffer
uint16_t x_map,y_map;                           // Variables to store mapped values
int speedl,speedr,offset;                       // Variables to store speeds

/*****thresholds*****/
uint8_t Ymin=0;
uint8_t Ymidl=124;
uint8_t Ymid=129;
uint8_t Ymidh=134;
uint8_t Ymax=255;
uint8_t Xmin=0;
uint8_t Xmidl=124;
uint8_t Xmid=129;
uint8_t Xmidh=134;
uint8_t Xmax=255;
uint8_t yspeed,xspeed;

void setup()  // put your setup code here, to run once: 
{ 
 Serial.begin(9600);
 pinMode(Left1,OUTPUT);
 pinMode(Left2,OUTPUT);
 pinMode(Right1,OUTPUT);
 pinMode(Right2,OUTPUT);
 pinMode(Left_PWM,OUTPUT);
 pinMode(Right_PWM,OUTPUT); 
  
 /*****Setting Initial speed to 0*****/
 digitalWrite(Left1,LOW);
 digitalWrite(Left2,LOW);
 digitalWrite(Right1,LOW);
 digitalWrite(Right2,LOW); 
 analogWrite(Left_PWM,0);
 analogWrite(Right_PWM,0);
}

void loop()// put your main code here, to run repeatedly: 
{
 while(Serial.read()!='a'){}
 y = Serial.read();
 delay(35);
 x = Serial.read();
 delay(35);
 x_map = map(x,Xmin,Xmax,0,255); 
 y_map = map(y,Ymin,Ymax,0,255);

  /*****Forward Movement*****/
 if(y_map <= Ymidl) 
 {
  digitalWrite(Left1,HIGH);
  digitalWrite(Left2,LOW);
  digitalWrite(Right1,HIGH);
  digitalWrite(Right2,LOW);
  speedl=map(y_map,Ymidl,0,0,255);
  speedr=map(y_map,Ymidl,0,0,255);
  
  if(x_map <= Xmidl)                                //Forward Left Turn
  {
   offset=map(x_map,Xmidl,0,0,255);
   speedl=speedl-offset;
   speedr=speedr+offset;
   
   /*****Checking speed limits*****/
   if(speedl < 0)
   {
    speedl=abs(speedl);
    if(speedl > 255)
    {
     speedl=255; 
    }
    digitalWrite(Left1,LOW);
    digitalWrite(Left2,HIGH);
   }
    
   if(speedr > 255)
   {
    speedr=255; 
   }
  }
  
  else if (x_map >= Xmidh)                            //Forward Right Turn
  {
   offset=map(x_map,Xmidh,255,0,255);
   speedl=speedl+offset;
   speedr=speedr-offset;

   /*****Checking speed limits*****/ 
   if(speedl > 255)
   {
    speedl=255;
   }
   
   if(speedr < 0)
   {
    speedr=abs(speedr);
    if(speedr > 255)
    {
     speedr=255; 
    }
    digitalWrite(Right1,LOW);
    digitalWrite(Right2,HIGH);  
   }
  }
  else
  {
    if(speedr > 255)
    {
     speedr=255; 
    }
    if(speedl > 255)
    {
     speedl=255; 
    }  
  } 
 }
 
 /*****Backward Movement*****/
 else if(y_map >= Ymidh)
 {
  digitalWrite(Left1,LOW);
  digitalWrite(Left2,HIGH);
  digitalWrite(Right1,LOW);
  digitalWrite(Right2,HIGH);
  speedl=map(y_map,Ymidh,255,0,255);
  speedr=map(y_map,Ymidh,255,0,255);
  
  if(x_map <= Xmidl)                                    //Backward Left Turn
  {
   offset=map(x_map,Xmidl,0,0,255);
   speedl=speedl-offset;
   speedr=speedr+offset;

   /*****Checking speed limits*****/
   if(speedl < 0)
   {
    speedl=abs(speedl);
    if(speedl > 255)
    {
     speedl=255; 
    }
    digitalWrite(Left1,HIGH);
    digitalWrite(Left2,LOW);
   } 
   if(speedr > 255)
   {
    speedr=255; 
   }
  }
  
  else if (x_map >= Xmidh)                              //Backward Right Turn
  {
   offset=map(x_map,Xmidh,255,0,255);
   speedl=speedl+offset;
   speedr=speedr-offset;

   /*****Checking speed limits*****/ 
   if(speedl > 255)
   {
    speedl=255;
   }
   if(speedr < 0)
   {
    speedr=abs(speedr);
    if(speedr > 255)
    {
     speedr=255; 
    }
    digitalWrite(Right1,HIGH);
    digitalWrite(Right2,LOW);  
   }
  }
  
  else
  {
    if(speedr > 255)
    {
     speedr=255; 
    }
    if(speedl > 255)
    {
     speedl=255; 
    } 
  }
 }

 /*****Anti-Clockwise Rotation*****/
 else if ((y_map >= Ymidl) && (y_map <= Ymidh) && (x_map <= Xmidl))
 {
  digitalWrite(Left1,HIGH);
  digitalWrite(Left2,LOW);
  digitalWrite(Right1,LOW);
  digitalWrite(Right2,HIGH);
  speedl=map(x_map,Xmidl,0,0,255);
  speedr=map(x_map,Xmidl,0,0,255);
  if(speedl>255)
  {
   speedl=255;
  }
  if(speedr>255)
  {
   speedr=255;
  }
 }

 /*****Clockwise Rotation*****/
 else if ((y_map >= Ymidl) && (y_map <= Ymidh) && (x_map >= Xmidh))
 {
  digitalWrite(Left1,LOW);
  digitalWrite(Left2,HIGH);
  digitalWrite(Right1,HIGH);
  digitalWrite(Right2,LOW);
  speedl=map(x_map,Xmidh,255,0,255);
  speedr=map(x_map,Xmidh,255,0,255);
  if(speedl>255)
  {
   speedl=255;
  }
  if(speedr>255)
  {
   speedr=255;
  }
 }

 /*****Stop*****/
 else
 {
  digitalWrite(Left1,LOW);
  digitalWrite(Left2,LOW);
  digitalWrite(Right1,LOW);
  digitalWrite(Right2,LOW);
  speedl=0;
  speedr=0;
 }

 /*****Set speeds*****/
 analogWrite(Left_PWM,speedl);
 analogWrite(Right_PWM,speedr);
}
