/********************************************************
  Alhad Manure
  Date: 09/12/2018
 *******************************************************/

int xright,yright;//variables containning joystic values
int xspeed,yspeed,speed1,speed2;//variables containning pwm values
char rotation;
unsigned int rotation_angle=10;// Still to be calculated 
const int fr_left=11;
const int fr_left_pwm=12;
const int fr_right=9;
const int fr_right_pwm=10;
const int rr_left=7;
const int rr_left_pwm=8;
const int rr_right=5;
const int rr_right_pwm=6;
const int mot[4]={11,9,7,5};
const int mot_pwm[4]={12,10,8,6};
void forward_moves(int yspeed1, int xright1);
void backward_moves(int yspeed1, int xright1);
void side_moves(int xright1);
void setspeed(void);
void setup() // put your setup code here, to run once: 
{
 unsigned int i; 
 Serial1.begin(9600);
 Serial.begin(9600);
 for(i=0;i<4;i++)
 {
  pinMode(mot[i],OUTPUT);//Setting motor driver as o/p pins
  //pinMode(mot_pwm[i],OUTPUT);//Setting motor driver pwm as o/p pins 
 }
}

void loop()// put your main code here, to run repeatedly:
{
  unsigned int i;
  Serial.write('s');
  if(Serial1.available()>=3) //Reception of i/p
  {
   xright=Serial1.read();
   delay(10);
   Serial.println("xright:");
   Serial.println(xright);
   yright=Serial1.read();
   delay(10);
   Serial.println("yright:");
   Serial.println(yright);
   rotation=Serial1.read();
   delay(10);
   Serial.println("yright:");
   Serial.println(yright);
  }
  if(rotation == 'n')
  {
   if(yright>135)//check whether a forward move
   {
    yspeed=map(yright,135,255,0,255);
    forward_moves(yspeed,xright);
   }
   else if(yright<120)//Check whether a Backward move
   {
    yspeed=map(yright,120,0,0,255);
    backward_moves(yspeed,xright);
   }
   else
   {
    yspeed=0;
    side_moves(xright);
   }
  } 
  else
  {
   if(rotation == 'l')
   {
    digitalWrite(fr_left,LOW);
    digitalWrite(rr_left,LOW);
    digitalWrite(fr_right,LOW); 
    digitalWrite(rr_right,LOW);
    speed1=127;
    speed2=127;
    setspeed();
    delay(rotation_angle);
    speed1=0;
    speed2=0;
   }
   else if(rotation == 'r')
   {
    digitalWrite(fr_left,HIGH);
    digitalWrite(rr_left,HIGH);
    digitalWrite(fr_right,HIGH); 
    digitalWrite(rr_right,HIGH);
    speed1=127;
    speed2=127;
    setspeed();
    delay(rotation_angle);
    speed1=0;
    speed2=0;
   }
  }
  setspeed(); 
 }
void forward_moves(int yspeed1,int xright1)//Function containning Forward motion
{
  int fyspeed, fxright, fxspeed;
  fxright=xright1;
  fyspeed = yspeed1;
  digitalWrite(fr_left,HIGH);
  digitalWrite(rr_left,HIGH);
  digitalWrite(fr_right,LOW); 
  digitalWrite(rr_right,LOW);
  if(fxright < 120)//Forward Right turn
  {
   fxspeed=map(fxright,120,0,0,255);
   speed1=fyspeed+fxspeed;
   speed2=fyspeed-fxspeed;
   if(speed1>255)
   {
    speed1=255; 
   }
   if(speed2<0)
   {
    digitalWrite(rr_left,LOW);
    digitalWrite(fr_right,HIGH);
    speed2=abs(speed2);
   } 
  }
  else if(fxright > 135)//Forward Left turn
  {
   fxspeed=map(fxright,135,255,0,255);
   speed1=fyspeed-fxspeed;
   speed2=fyspeed+fxspeed;
   if(speed2 > 255)
   {
    speed2=255; 
   }
   if(speed1<0)
   {
    digitalWrite(fr_left,LOW);
    digitalWrite(rr_right,HIGH);
    speed1=abs(speed1); 
   }
  }
  else
  {
   fxspeed=0;
   speed1=fyspeed;
   speed2=fyspeed;   
  } 
}
void backward_moves(int yspeed1, int xright1)
{
 int byspeed,bxspeed,bxright;
 byspeed=yspeed1;
 bxright=xright1;
 digitalWrite(fr_left,LOW);
 digitalWrite(rr_left,LOW);
 digitalWrite(fr_right,HIGH); 
 digitalWrite(rr_right,HIGH);
 if(bxright > 135)//Backward Left
 {
  bxspeed=map(bxright,135,255,0,255);
  speed1=byspeed+bxspeed;
  speed2=byspeed-bxspeed;
  if(speed1>255)
  {
   speed1=255; 
   }
   if(speed2<0)
   {
    digitalWrite(fr_right,LOW);
    digitalWrite(rr_left,HIGH);
    speed2=abs(speed2); 
   } 
 }
 else if(bxright < 120)//Backward Right
 {
  bxspeed=map(bxright,120,0,0,255);
  speed1=byspeed-bxspeed;
  speed2=byspeed+bxspeed;
  if(speed2 > 255)
  {
   speed2=255; 
  }
  if(speed1<0)
  {
   digitalWrite(fr_left,HIGH);
   digitalWrite(rr_right,LOW);
   speed1=abs(speed1); 
  }
 }
 else
 {
  bxspeed=0;
  speed1=byspeed;
  speed2=byspeed;   
 }  
}
void side_moves(int xright1)
{
 int sxright;
 sxright=xright1;
 if(sxright < 120)//Right movement
 {
  digitalWrite(fr_left,HIGH);
  digitalWrite(rr_left,LOW);
  digitalWrite(fr_right,HIGH);
  digitalWrite(rr_right,LOW);
  speed1=map(sxright,120,0,0,255);
  speed2=map(sxright,120,0,0,255);
 }
 else if(sxright > 135)// left movement
 {
  digitalWrite(fr_left,LOW);
  digitalWrite(rr_left,HIGH);
  digitalWrite(fr_right,LOW);
  digitalWrite(rr_right,HIGH);
  speed1=map(sxright,135,255,0,255);
  speed2=map(sxright,135,255,0,255);  
 }
 else
 {
  speed1=0;
  speed2=0; 
 }  
}
void setspeed()
{
 analogWrite(fr_left_pwm,speed1);
 analogWrite(fr_right_pwm,speed2);
 analogWrite(rr_left_pwm,speed2);
 analogWrite(rr_right_pwm,speed1);
}   
