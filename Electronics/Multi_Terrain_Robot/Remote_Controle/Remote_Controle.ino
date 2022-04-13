const int Y_pot=A0;
const int X_pot=A1;
const int Bat=A11;
const int Ble=23;
uint16_t Y, X, speedo, bat_charge, bat_avg[20];
uint8_t bat_ind;
uint8_t Xpos,Ypos;
uint16_t Ymid=513, Xmid=513;
uint16_t Ymax=765, Xmax=773;
uint16_t Ymin=253, Xmin=246;
uint16_t Xdiff=0, Ydiff=0;

unsigned int avg(uint16_t arr[], uint16_t sze)
{
 unsigned int sum;
 uint8_t i;
 for(i=0; i<sze; i++)
 {
  sum+=arr[i]; 
 }
 return sum/sze;  
}

void end_string()
{
 Serial3.write(0xff);
 Serial3.write(0xff);
 Serial3.write(0xff);
}

void read_bat_volt(uint8_t ind, uint16_t arr[], int pin)
{
  if(ind >= 20)
  {
    ind = 0;
  }
  bat_avg[ind]=analogRead(pin);
  ind++;
}

void setup()// put your setup code here, to run once:
{
  uint8_t i;
  Serial3.begin(9600);
  Serial.begin(9600);
  pinMode(Y_pot,INPUT);
  pinMode(X_pot,INPUT);
  pinMode(Bat,INPUT);
  pinMode(Ble,INPUT);
  
  for(i=0;i<20;i++)
  {
   bat_avg[i]=analogRead(Bat);
   delay(10);
  }
  Serial3.print("j0.val=");
  Serial3.print(avg(bat_avg, 20));
  end_string();  
}

//Y=765, 493, 253 
//X=740, 493, 247

void loop()// put your main code here, to run repeatedly:
{
 Y=analogRead(Y_pot);
 X=analogRead(X_pot);
 /*Serial.println("Unmapped:");
 Serial.println(Y);
 Serial.println(X);*/
 Ypos=map(Y,Ymin,Ymax,0,255);
 Xpos=map(X,Xmin,Xmax,0,255);
 /*Serial.println("Mapped:");
 Serial.println(Ypos);
 Serial.println(Xpos);*/
 Serial.write('a');
 Serial.write(Ypos);
 delay(35);
 Serial.write(Xpos);
 delay(35);
 
 Ydiff = abs(Y-Ymid);
 Xdiff = abs(X-Xmid); 
 uint16_t speedo = Xdiff+Ydiff;
 if(speedo>(Ymax-Ymid))
 {
  speedo=(Ymax-Ymid);
 }
 speedo = map(speedo, 0, Ymax-Ymid, 0, 220);
 if(speedo>=0 && speedo<=40)
 {
  speedo+=320;
 }
 else
 {
  speedo = map(speedo,40,220,0,220);
 }

 Serial3.print("z0.val=");
 Serial3.print(speedo);
 end_string();
 
 read_bat_volt(bat_ind, bat_avg, Bat);
 bat_charge = avg(bat_avg, 20);
 bat_charge = map(bat_charge,706,1023,0,100);
 Serial3.print("j0.val=");
 Serial3.print(bat_charge);
 end_string();
 
 Serial3.print("n0.val=");
 Serial3.print(bat_charge);
 end_string();
 
 if(digitalRead(Ble) == HIGH)
 {
  Serial3.print("t0.txt=");
  Serial3.print("\"");
  Serial3.print("Connected");
  Serial3.print("\"");
  end_string();  
 }
 else
 {
  Serial3.print("t0.txt=");
  Serial3.print("\"");
  Serial3.print("Not Connected");
  Serial3.print("\"");
  end_string();
 }
}
