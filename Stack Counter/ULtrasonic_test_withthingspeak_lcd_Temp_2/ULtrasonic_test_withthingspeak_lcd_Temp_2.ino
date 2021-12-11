#include <CytronWiFiShield.h>
#include <CytronWiFiClient.h>
#include <SoftwareSerial.h>
#include <ThingSpeak.h>
#include<LiquidCrystal.h>
LiquidCrystal lcd(52,50,34,32,30,28);
const char *ssid = "Crackit";
const char *pass = "aallhhuurree007";//7XRC6YPY7KIGYD5O
const char* server = "api.thingspeak.com";
String apiKey ="7XRC6YPY7KIGYD5O";
ESP8266Client client; 
const int trigPin[] = {23,27,31,35,39,43};// array containning trigger pins for Ultrasonic sensor
const int echoPin[] = {25,29,33,37,41,45};// array containning echopin pins for Ultrasonic sensor
const int lm = A15;//analog pin for LM35
float temp;// Room temperature 
double vel;//Sound velocity at given temperature
String destination;//string containning name of customer
String number;//string containning number of Truck
String manual_count;//Manual count of sacks   
unsigned int i,H,L;// index variable,Height
unsigned int flg,flg1;// flag indicating whether process is in progress or not
unsigned int grid[5][5];//2d array containning values of i th row and j th column
unsigned int c=2.5;//Width of Cement sack 
long duration;//duration of travelling of trigger pulse
int distance;//distance measured by ultrsonic sensor
char cmd;//command variable
unsigned int distance_measure(unsigned int i);// function to measure distance
void truck_measurement();//function to measure truck's compartment length and breadth
void scan_i_th_row(unsigned int k);//function scanning columns from i_th row  
void truck_details();//function to i/p truck details
void command();//function to assign next command
void Event1(char c);//function containning processes
void remove_garbage();// function clearing serial buffer
unsigned int count(unsigned int h);//function counting sacks in given column
void sendconfirmation(String no, unsigned int rreess, String manual);//function uploading data to Thingspeak
void calibrate_temp();//Function deciding velocity of sound at given temperature
void setup()
{
  Serial.begin(9600); // Starts the serial communication at 9600baud rate
  lcd.begin(20,4);//initialise 20 columns by 4 rows lcd
  lcd.setCursor(0,0);
  lcd.print("Sparkhound          ");
  if(!wifi.begin(10,11))//beginning serial communication with WiFi 
  {
    Serial.println(F("Error talking to shield"));
    lcd.setCursor(0,0);
    lcd.print("Error                ");
    while(1);//Wait till Serial communication starts
  }
  Serial.println(F("Start wifi connection"));
  lcd.setCursor(0,0);
  lcd.print("Start wifi connection");
  if(!wifi.connectAP(ssid, pass))//Connect to WiFi containning given SSID and Password
  {
    Serial.println(F("Error connecting to WiFi"));
    lcd.setCursor(0,0);
    lcd.print("Error                ");
    while(1);//wait till WiFi connection is established
  } 
  Serial.print(F("Connected to "));
  Serial.println(wifi.SSID());
  lcd.setCursor(0,0);
  lcd.print("Connected to        ");
  lcd.setCursor(0,1);
  lcd.print(wifi.SSID());
  Serial.println(F("IP address: "));
  Serial.println(wifi.localIP()); 
  lcd.setCursor(0,2);
  lcd.print(wifi.localIP());
  delay(2000);
  for(i=0;i<4;i++)// make all trigPin as an Output & echoPin as an Input
  {
   pinMode(trigPin[i], OUTPUT); // Sets the trigPin as an Output
   pinMode(echoPin[i], INPUT); // Sets the echoPin as an Input
  }
  flg1=0;
  pinMode(lm,INPUT); 
  calibrate_temp();//Find sound velocity at given temperature
}

void loop()
{
 char ch;
 if(flg1==0)
 { 
  Serial.println("Sparkhound");
  Serial.println("Press m for menu");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sparkhound          ");
  lcd.setCursor(0,1);
  lcd.print("Press m for menu: ");
  delay(2000);
 } 
 flg1=1;//to print above statement only once in loop  
 remove_garbage();
 while(!Serial.available()){}//wait till serial data is available 
 ch=Serial.read();//read serial character received
 Serial.println(ch);
 lcd.write(ch);
 delay(2000);
 lcd.clear(); 
 switch(ch)//switch case to print menu and start process 
  {    
   case 'm':
      Serial.println("Menu:");
      delay(100);
      lcd.setCursor(0,0);
      lcd.print("Menu:");
      delay(200);
      Serial.println("  s:Start");
      delay(100);
      lcd.setCursor(8,1);
      lcd.print("s:Start");
      delay(200);
      Serial.println("  b:Exit");
      delay(100);
      lcd.setCursor(8,2);
      lcd.print("b:Exit");
      lcd.setCursor(0,3);
      lcd.print("Option: ");
      delay(2000);
      remove_garbage();
      break;
   case 's':
      Event1('s');
      break;
   case 'b':
      flg1=0;
      break;   
   default:
      Serial.println("Invalid I/P");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Invalid I/P");
      delay(2000);   
 } 
}

void calibrate_temp()//function to find the current speed of sound in air
{
 float vout; // analog voltage at lm35
 Serial.println("Calibrating Device");
 vout=analogRead(lm);//Reading analog voltage 
 temp=((vout*500)/1023);// Temperature in degree celcius
 vel=round(321+(0.6*temp));// Current velocity of sound
 vel=vel/10000;
 Serial.println("Vel:");
 Serial.println(vel);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Velocity:");
 lcd.print(vel);
 delay(2000);
}

void sendconfirmation(String no, unsigned int rreess, String manual)
{  
   lcd.clear();
   if (client.connect(server, 80))//Begin communication with Thingspeak server process
   { // use ip 184.106.153.149 or api.thingspeak.com
    Serial.println("WiFi Client connected ");
    lcd.setCursor(0,0);
    lcd.print("WiFi Client connected");
    String postStr = apiKey;//Format for writing data on Thingspeak
    postStr += "&field1=";
    postStr += String(no);
    postStr += "&field2=";
    postStr += String(rreess);
    postStr += "&field3=";
    postStr += String(manual);
    postStr += "\r\n\r\n\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    delay(2000);
   }//end if
 client.stop();
}

unsigned int distance_measure(unsigned int i)
{
  digitalWrite(trigPin[i],LOW);//Apply pulse to i th Ultrasonic sensors trigger pin
  delayMicroseconds(2);
  digitalWrite(trigPin[i],HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin[i],LOW);
  duration = pulseIn(echoPin[i],HIGH);//Measure duration till echo pin is high 
  distance= (duration/2)*vel;//distance calculation from obstacle
  Serial.println("Distance: ");
  Serial.print(i);
  Serial.println(distance);
  delay(1000);
  return distance; 
}
unsigned int count(unsigned int h)
{
 Serial.println("in count");  
 unsigned int res;
 res=((H-h)/c); 
 return res;//sacks count in given column
 
} 
void truck_measurement()
{ 
 /*Serial.println("Length: ");
 lcd.setCursor(0,1);
 lcd.print("Length: ");
 L=distance_measure(i=1);//Length measurement
 Serial.println(L);
 lcd.print(L);*/
 Serial.println("Height:");
 lcd.setCursor(0,2);
 lcd.print("Height: ");
 H=distance_measure(i=3);//Height measurement
 Serial.println(H);
 lcd.print(H);
 delay(4000);  
}
void scan_i_th_row(unsigned int k)
{
  lcd.clear();
  unsigned int j;
  for(j=0;j<4;j++)  //Scan columns of i th row
  {
   Serial.println("Scanning ");
   Serial.print(j+1);
   Serial.print("th column");
   lcd.setCursor(0,0);
   lcd.print("Scanning ");
   lcd.print(j+1);
   lcd.print("th col   ");
   delay(2000);
   grid[k][j]=distance_measure(j);
   if(grid[k][j] > H)
   {
    grid[k][j]=H;//if distance of empty sack is little more than total compartment height make it equal to compartment height 
   }
   lcd.setCursor(0,1);
   lcd.print("Distance:");
   lcd.setCursor(0,2);
   lcd.print(grid[k][j]);
   delay(2000); 
  }
}
void command()
{
  char ch2;
  flg=flg+1;
  while(!Serial.available()){}
  ch2=Serial.read();
  Serial.println("Ch2");
  lcd.setCursor(0,1);
  lcd.write(ch2);
  remove_garbage();
  Serial.println(ch2); 
  switch(ch2)
  {
    case 'N'://Start next process
           {
            switch(flg)
            {
              case 1:
                 cmd='r';
                 break;
              case 2:
                 cmd='l';
                 break;
              default:
                 cmd='b';   
            }
            break;
           }
     case 'n'://Scan next row 
           {
            flg=2;
            cmd='l';
            break;
           }     
     case 'b'://Exit
           {
            cmd='b';
            break;       
           }
     case 'Y'://Retry process if exit by fault or mistake
           {
             flg=0;
             cmd='s';
             break;
           }    
     default:
           {
            Serial.println("flg");
            Serial.println(flg);
            Serial.println("Invalid I/P");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Invalid I/P");
            delay(2000);
            flg=flg-1;      
           }
  }
}
void remove_garbage()
{
  char gar;
  while(Serial.available()!=0)
  {
   gar=Serial.read(); 
  }
}
void truck_details()
{
 lcd.clear(); 
 String str;   
 Serial.println("Enter Destination:");
 lcd.setCursor(0,0);
 lcd.print("Enter Destination:");
 Serial.setTimeout(7000);//wait for 7 seconds to accept serial data
 remove_garbage();
 while(!Serial.available()){}
 destination=Serial.readString();   
 Serial.println(destination);
 lcd.setCursor(0,1);
 lcd.print(destination);
 Serial.println("Enter Number:");
 lcd.setCursor(0,2);
 lcd.print("Enter Truck No.:");
 while(!Serial.available()){}
 number=Serial.readString(); 
 Serial.println(number);
 lcd.setCursor(0,3);
 lcd.print(number);
 delay(2000);
 Serial.println("Enter Manual count:");
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Enter Man. cnt: ");
 while(!Serial.available()){}
 manual_count=Serial.readString(); 
 Serial.println(manual_count);
 lcd.setCursor(0,1);
 lcd.print(manual_count);
 delay(2000);
}
void Event1(char c)
{
    char ch1;
    unsigned int k,v,w;
    unsigned int tmp;
    unsigned int result=0;
    unsigned int nnuumm;//vaiable containning manual sack count in integer format
    lcd.clear();
    Serial.println("You are in Serial"); 
    cmd=c;
    Serial.print(cmd);
        while(cmd!='E')
        {
         switch(cmd)
         {
            case 's':  
                        if (flg==0)
                        {    
                         Serial.println("Enter Truck Details: ");
                         lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("Truck Details:  ");
                         truck_details();
                         delay(2000);
                         Serial.println("Enter N to next or b for Exit");
                         lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("N: Next b: Exit ");
                         remove_garbage();
                         command();//accept next command
                         delay(2000);
                         Serial.println("cmd");
                         Serial.println(cmd);
                         Serial.println("flg");
                         Serial.println(flg);
                        } 
                        break;
            case 'r':  
                        lcd.clear();
                        if(flg == 1)
                        {
                         Serial.println("Measuring Truck Dimensions");
                         lcd.setCursor(0,0);
                         lcd.print("Truck Dimensions: ");
                         delay(2000);
                         truck_measurement();
                         Serial.println("flg");
                         Serial.println(flg);
                         k=0;//initialise counting row index for first row
                        }
                        remove_garbage();
                        Serial.println("Enter N to next or b for Exit");
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("N: Next b: Exit ");
                        command();//accept next command
                        delay(2000);  
                        break;
            case 'l':   
                        if(flg==2)
                        {
                         scan_i_th_row(k);//scan each column of k+1 th row
                         Serial.println("Scanning of ");
                         Serial.println(k+1);
                         Serial.println(" th row done");
                         lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("Scanning of     ");
                         lcd.setCursor(0,1);
                         lcd.print(k+1);
                         lcd.print(" th row done  ");
                         delay(2000);
                         k++;
                        }
                        lcd.clear(); 
                        Serial.println(" Press n for next row b for Exit");
                        lcd.setCursor(0,0);
                        lcd.print("n: N_row b: Exit");
                        remove_garbage();
                        command();//accept next command
                        delay(2000);   
                        break;                                 
            case 'b':   
                       if(flg==3)
                       {
                        for(v=0;v<k;v++)
                        {
                         for(w=0;w<4;w++)
                         {
                          lcd.setCursor(0,0);
                          lcd.print(w);
                          tmp=count(grid[v][w]);
                          lcd.clear();
                          lcd.setCursor(0,1);
                          lcd.print(tmp);
                          delay(2000);
                          result=result + tmp ;
                         } 
                        }
                        Serial.print("Final Count: ");
                        Serial.println(result);
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Final Count:    ");
                        lcd.setCursor(0,1);
                        lcd.print(result);
                        delay(2000);
                        sendconfirmation(number, result, manual_count);//Upload data to Thingspeak
                        nnuumm = manual_count.toInt();//Convert data in string format to integer 
                        destination.remove(0);//clear string
                        number.remove(0);//clear string
                        if(nnuumm == result)//Verify
                        { 
                         Serial.println("Confirmed");
                         lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("Confirmed       ");
                         delay(2000);  
                        }
                        if(flg == 3)//process successfully completed
                        {
                         Serial.println("Process Complete");
                         cmd='E';
                         lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("Process Complete");
                         delay(3000);
                         Serial.println("Thank You");
                         lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("Thank You");
                         delay(3000);
                        } 
                       }
                       else//process interrupted
                       {
                         remove_garbage();
                         Serial.println("Press Y to retry or b to exit");
                         lcd.setCursor(0,0);
                         lcd.print("Y: Retry b: Exit");
                         delay(2000);
                         remove_garbage();
                         command();
                         if(cmd == 'b')
                         {
                          Serial.println("Thank You");
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Thank You");
                          cmd='E';
                         }   
                        }  
                        flg=0;//clear flags 
                        flg1=0;
                        break;
            default:// if command is invalid reenter the command
                       if(flg == 2)
                       {
                        lcd.clear();
                        Serial.println("Press n for next row otherwise b for Exit");
                        lcd.setCursor(0,0);
                        lcd.print("n: N_row b: Exit");
                        delay(2000);
                       }
                       else
                       {
                        Serial.println("Enter N to next or b for Exit");
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("N: Next b: Exit  ");
                        delay(2000);
                       }
                       remove_garbage();
                       cmd=Serial.read();
                       command();       
          }
     } 
}     
