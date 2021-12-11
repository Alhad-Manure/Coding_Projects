const int trigPin[] = {2,4,6,8};// array containning trigger pins for Ultrasonic sensor
const int echoPin[] = {3,5,7,9};// array containning echopin pins for Ultrasonic sensor
String destination;//string containning name of customer
String number;//string containning number of Truck   
unsigned int i,H,L;// index variable
unsigned int flg,flg1;// flag indicating whether process is in progress or not
unsigned int grid[4][4];//2d array containning values of i th row and j th column
unsigned int c;//Width of Cement sack 
long duration;//duration of travelling of trigger pulse
int distance;//
char cmd;
unsigned int distance_measure(unsigned int i);// function to measure distance
void truck_measurement();//function to measure truck's compartment length and breadth
char scan_i_th_row(unsigned int k);//function scanning columns from i_th row  
void truck_details();//function to i/p truck details
char command();
void Event1(char c);
void remove_garbage();
unsigned int count(unsigned int h);
void setup()
{
 for(i=0;i<4;i++)// make all trigPin as an Output & echoPin as an Input
 {
  pinMode(trigPin[i], OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin[i], INPUT); // Sets the echoPin as an Input
 } 
 Serial.begin(9600); // Starts the serial communication at 9600baud rate
 flg1=0;
}

void loop()
{
 char ch;
 if(flg1==0)
 { 
  Serial.println("Sparkhound");
  Serial.println("Press m for menu");
  delay(50);
 } 
 flg1=1; 
 
 remove_garbage();
 while(!Serial.available()){} 
 ch=Serial.read();
 Serial.println(ch); 
 switch(ch)
  {    
   case 'm':
      Serial.println("Menu:");
      delay(100);
      Serial.println("  s:Start");
      delay(100);
      Serial.println("  b:Exit");
      delay(100);
      remove_garbage();
      break;
   case 's':
      Event1('s');
      break;
   default:
      Serial.println("Invalid I/P");   
 } 
}

unsigned int distance_measure(unsigned int i)
{
  digitalWrite(trigPin[i],LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin[i],HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin[i],LOW);
  duration = pulseIn(echoPin[i],HIGH);
  distance= (duration/2)*0.034;
  Serial.println("Distance: ");
  Serial.print(i);
  Serial.println(distance);
  delay(1000);
  return distance; 
}
unsigned int count(unsigned int h)
{
 unsigned int res,total;
 return res=((H-h)/c);
} 
void truck_measurement()
{
 Serial.println("Length: ");
 L=distance_measure(i=0);//Length measurement
 Serial.println(L);
 Serial.println("Height:");
 H=distance_measure(i=2);//Breadth measurement
 Serial.println(H);  
}
char scan_i_th_row(unsigned int k)
{
  unsigned int j;
  i=k;
  for(j=0;j<4;j++)  //Scan columns of i th row
  {
   Serial.println("Scanning ");
   Serial.print(j);
   Serial.print("th column");
   grid[i][j]=distance_measure(j); 
  }
  return 'D';
}
char command()
{
  char cmda,ch2;
  flg=flg+1;
  while(!Serial.available()){}
  ch2=Serial.read();
  Serial.println("Ch2");
  remove_garbage();
  Serial.println(ch2); 
  switch(ch2)
  {
    case 'N':
           {
            switch(flg)
            {
              case 1:
                 cmda='r';
                 break;
              case 2:
                 cmda='l';
                 break;
              default:
                 cmda='b';   
            }
            break;
           }
     case 'n':
           {
            flg=2;
            cmda='l';
            break;
           }     
     case 'b':
           {
            cmda='b';
            break;       
           }
     default:
           {
            Serial.println("flg");
            Serial.println(flg);
            Serial.println("Invalid I/P");
            flg=flg-1;      
           }
  }
  return cmda;
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
 String str;   
 Serial.println("Enter Destination:");
 Serial.setTimeout(10000);
 remove_garbage();
 while(!Serial.available()){}
 destination=Serial.readString();   
 Serial.println(destination);
 Serial.println("Enter Number:");
 while(!Serial.available()){}
 number=Serial.readString(); 
 Serial.println(number);
 return;
}
void Event1(char c)
{
    char done,ch1;
    unsigned int k,v,w;
    unsigned int  result=0;
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
                         truck_details();
                         Serial.println("Enter N to next or b for Exit");
                         remove_garbage();
                         cmd=command();
                         Serial.println("cmd");
                         Serial.println(cmd);
                         Serial.println("flg");
                         Serial.println(flg);
                        } 
                        break;
            case 'r':  
                        if(flg == 1)
                        {
                         Serial.println("Measuring Truck Dimensions");
                         truck_measurement();
                         Serial.println("flg");
                         Serial.println(flg);
                         k=0;
                        }
                        else
                        {
                         Serial.println("Error");
                         cmd='b'; 
                        }
                        remove_garbage();
                        Serial.println("Enter N to next or b for Exit");
                        cmd=command();  
                        break;
            case 'l':   
                        if(flg==2)
                        {
                         done=scan_i_th_row(k);
                         if(done=='D')
                         {
                          Serial.println("Sanning of ");
                          Serial.println(k+1);
                          Serial.println(" th row done");
                          k++;
                         }
                        } 
                        Serial.println(" Press n for next row otherwise N for next b for Exit");
                        remove_garbage();
                        cmd=command();   
                        break;                                 
            case 'b':   
                       if(flg==3)
                       {
                        for(v=0;v<4;v++)
                        {
                         for(w=0;w<4;w++)
                         {
                          result=result + count(grid[v][w]);
                         } 
                        }
                        Serial.print("Final Count: ");
                        Serial.println(result);
                        Serial.println("Process Complete");
                        destination.remove(0);
                        number.remove(0);
                        cmd='E';
                       }
                       else
                       {
                         remove_garbage();
                         Serial.println("Press Y to retry or b to exit");
                         remove_garbage();
                         while(!Serial.available()){}
                         ch1=Serial.read();
                         Serial.println("ch1");
                         Serial.println(ch1);
                         if(ch1 == 'Y')
                         {
                          cmd='s'; 
                         } 
                         else if(ch1 == 'b')
                         {
                          Serial.println("Thank You");
                          cmd='E';
                         }   
                        }  
                        flg=0;
                        flg1=0;
                        break;
            default:
                       if(flg == 2)
                       {
                        Serial.println("Press n for next row otherwise b for Exit");
                       }
                       else
                       {
                        Serial.println("Enter N to next or b for Exit");
                       }
                       remove_garbage();
                       cmd=Serial.read();
                       cmd=command();       
          }
     } 
}     
