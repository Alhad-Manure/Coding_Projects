/*
 * GccApplication1.c
 *
 * Created: 11-Apr-19 8:41:04 AM
 * Author : ALHAD MANURE
 */ 

#include "head.h"

ISR(INT0_vect)								//Set the cell count
{ 
 uint8_t i;	   	
 if(str_stp == 0xFF)						//If charging is in progress stop it first before setting new cell count
 {
  wrcmd(0x01);
  wrcmd(0x80);	 
  lcd_print("First stop the  ");
  wrcmd(0xC0);
  lcd_print("Charging");
  _delay_ms(500);
 }
 else										//If charging is not in progress set the cell count for charging
 {	
  s++;
  if(s > 3)
  {
   s = 1;	 
  }
  switch(s)
  {
	case 1:
	{
	 mcp_write(0x11, 66);				    //AD3:AD0 + C1:C0 + D9:D8 + D7:D0  (According to datasheet Address 0x00 for wiper 0 and write data command is c1:C0 = 00 
	 wrcmd(0x01);
	 wrcmd(0x80);
	 lcd_print("1S Battery mode ");
	 break;	
	}
	case 2:
	{
	 mcp_write(0x11, 149);
	 wrcmd(0x01);
	 wrcmd(0x80);
	 lcd_print("2S Battery mode ");
	 break;
	}
	case 3:
	{
	 mcp_write(0x11, 235);
	 wrcmd(0x01);
	 wrcmd(0x80);
	 lcd_print("3S Battery mode ");
	 break;
	}  
   }
   for(i=0;i<10;i++)
   {
	measure_bat_voltage();						//Find current Battery voltage 
   }
   start=1;										//Flag to confirm whether the Battery count is set.
 }
 EIFR |= 1<<INTF0;								//Clear the External interrupt 0 flag
}

ISR(INT1_vect)									// Start or Stop charging
{
 str_stp ^= 0xFF;								//Complement the last state if charging is on turn it off and vice versa
 if(str_stp == 0xFF)							
 {
  wrcmd(0x01);
  wrcmd(0x80); 
  if(start == 1)								//Check whether cell count is already set if not return
  {
   lcd_print("Charging On     ");  
  }
  else
  {
   lcd_print("!Set cell count!");
   str_stp = 0x00;	  
  }	   	 	 
 }
 else if(str_stp == 0x00)						
 {
  wrcmd(0x01);
  wrcmd(0x80);
  lcd_print("Charging OFF"); 
 }									
 EIFR |= 1<<INTF1;							//Clear the External interrupt 1 flag
 _delay_ms(500);  	
}			

void mcp_write(char cmd, char data)			//Function to send SPI command 
{	
 PORTB &= 0XFB;								//Set slave select pin to logic 0 to start SPI communication with MCP4131
 SPDR = cmd;								//Send Upper 8-bits of 16 bit command, command = Address + Command
 while((SPSR & 0x80) != 0x80){}				//wait till transmission is complete ** SPIF  WCOL –  –  –  –  –  SPI2X **
 SPDR = data;			     				//Send Lower 8-bits of 16 bit command, Data
 while((SPSR & 0x80) != 0x80){}				//wait till transmission is complete
 PORTB |= 0X04;								//Set slave select pin to logic 1 to stop SPI communication with MCP4131	 
}

void genen()								//Function to create pulse at LCD Enable pin
{
 PORTB &= 0b11111101;						//Enable pin is made LOW
 _delay_ms(1);
 PORTB |= 0b00000010;						//Enable pin is made HIGH
 _delay_ms(1);
 PORTB &= 0b11111101;						//Enable pin is made LOW
}

void wrcmd(char c)							//Function to write command to LCD
{
 PORTB &= 0b11111110;						//Set LCD RS pin to logic 0
 PORTC = (((c & 0xf0)>>2) | 0x03);			//Send Upper 4 bits of command keeping External interrupt 0 & External interrupt 1 pins as input pins with pull up resistor active 
 genen();
 PORTC = (((c & 0x0f)<<2) | 0x03);			//Send Lower 4 bits of command keeping External interrupt 0 & External interrupt 1 pins as input pins with pull up resistor active
 genen();
 _delay_ms(2);	
}

void wrdata(char c)							//Function to write data to LCD
{
 PORTB |= 0b00000001;						//Set LCD RS pin to logic 1
 PORTC = (((c & 0xf0)>>2) | 0x03);			//Send Upper 4 bits of data keeping External interrupt 0 & External interrupt 1 pins as input pins with pull up resistor active
 genen();
 PORTC = (((c & 0x0f)<<2) | 0x03);			//Send Lower 4 bits of data keeping External interrupt 0 & External interrupt 1 pins as input pins with pull up resistor active
 genen();
 _delay_ms(2);
}

void lcd_print(char *ptr)
{
 while(*ptr)                                //Loop still string doesn't ends
 {
  wrdata(*ptr);								//Write current character to LCD
  ptr++;                                    //Increment Pointer position
 }
}

void lcd_init()								//Function to Initialize LCD
{
 _delay_ms(15);
 wrcmd(0x3F);								//Function set command
 _delay_ms(5);
 wrcmd(0x3F);								//Function set command
 _delay_ms(1);
 wrcmd(0x3F);								//Function set command
 wrcmd(0x02);								//4 bit lcd interface
 wrcmd(0x28);								//4 bit lcd interface with 2 rows 
 wrcmd(0x08);								//display off command
 wrcmd(0x01);								//clear display command
 wrcmd(0x06);								//
 wrcmd(0x0c);								//display on command
 wrcmd(0x80);								//set cursor to row 0 and column 0
}

float ADC_result(char i)					//Function to read voltage at ADC pin
{
 float tmp;
 ADMUX = ((ADMUX & 0xF0) | (i & 0x0F));		//Select the ADC channel; voltage = 0, current = 1; **REFS1  REFS0  ADLAR  –  MUX3  MUX2  MUX1  MUX0 
 ADCSRA |= 0x40;							//Start the reading **ADEN     ADSC    ADATE    ADIF      ADIE    ADPS2    ADPS1    ADPS0
 while((ADCSRA & 0x10)!=0x10){}				//Wait still reading is complete
 ADCSRA |= 0x10;							//Clear ADIF flag
 tmp = (ADCL | (ADCH << 8));				//read 10 bit ADC result
 return tmp;								//Return the result
}

float averager(uint8_t flg_1)				//Function to take average of last 10 ADC readings to obtain stable & faithful reading  
{
 uint8_t j;
 float res=0;	
 if(flg_1 == 1)								//If flg_1==1 then take average of Charging current
 {
  current_avg[I] = charge_current;			//Add last reading of ADC to next position of array of last 10 readings
  I++;
  if(I == 10)                               //Check if index overflows 
  {
   I = 0;
  }
  for(j=0;j<10;j++)
  {
   res += current_avg[j];					//Sum last 10 Charging current the results  
  } 
 }
 else if(flg_1 == 2)						//If flg_1 ==1 then take average of Battery voltage
 {
  voltage_avg[V] = battery_voltage;			//Add last reading of ADC to next position of array of last 10 readings
  V++;
  if(V == 10)								//Check if index overflows
  {
   V = 0;
  }
  for(j=0;j<10;j++)
  {
   res += voltage_avg[j];					//Sum last 10 Battery voltage results
  } 	 
 }
 res = (res / 10);						    //Calculate average of results
 return res;								//Return the result
} 

void measure_bat_voltage()							//Function to find current Battery voltage
{
 battery_voltage = ADC_result(0);					//ADC pin 0 for Battery voltage 
 battery_voltage = s * 0.0048 * battery_voltage;    //S value i.e. battery count is set in External interrupt 1
 cur_avg_volt = averager(2);						//Average of last ten battery voltage readings i.e actual current battery voltage
 _delay_ms(50); 
}

void measure_charge_current()						//Function to find current Charging current
{
 charge_current = ADC_result(1);					//ADC pin 1 for Charging current i.e actual current battery charging current
 charge_current = 0.0269 * (charge_current - 512);	//Converting analog output of ACS712-5B to actual current value
 cur_avg_curr = averager(1);						//Average of last ten charging current readings 
}

void mode_select()									//Function to automatically decide the current mode of charging 
{
 if(str_stp == 0x00)								//If charging is not ON just wait and display menu
 {
  wrcmd(0x01);
  wrcmd(0x80);
  lcd_print("Menu-");
  lcd_print(" Mode: key1");
  wrcmd(0xC5);
  lcd_print("Start: key2");
  _delay_ms(20); 
 }  	
 else if(str_stp == 0xFF && start == 1)				//Checking Battery status
 {
	 
  wrcmd(0x01);
  wrcmd(0x80);
  lcd_print("Battery");
  _delay_ms(200);	 
  wrcmd(0x01);
  wrcmd(0x80);	 	 	
  if((cur_avg_volt / s) <= 1)					    //Check if battery is over-discharged or not connected
  {
   PORTD |= 1<<5;									//Keep charging OFF
   lcd_print("Battery not     ");
   _delay_ms(20);
   wrcmd(0xC0);
   lcd_print("connected       ");
   measure_bat_voltage();						    //Keep checking current battery voltage	  
  } 	  
  else if((cur_avg_volt / s) <= 3)				    //If Over discharged battery cut off the charging
  {
   PORTD |= 1<<5;									//Keep charging OFF	  	  
   lcd_print("Damaged Battery ");
   _delay_ms(20);
   wrcmd(0xC0);
   lcd_print("Charging OFF ");
   measure_bat_voltage();						   //Keep checking current battery voltage 
  }
  else if((cur_avg_volt / s) < 4.1)			       //If battery is in good condition and voltage is less than 4.1V start charging in CC mode
  {
   while((cur_avg_volt / s) < 4.1)
   {  
    wrcmd(0x01);
    wrcmd(0x80);
    lcd_print("CC Mode");
    _delay_ms(200);
    PORTD &= 0x9F;								   //Turning on charging in CC mode 
    measure_bat_voltage();
    measure_charge_current();
    if(str_stp == 0x00)							   //If charging turned OFF through External Interrupt then come out of CC mode and turn off charging
    {
	 PORTD |= 3<<5;								   //Turning OFF charging 
	 break;
    }
   }
  }
  else if((cur_avg_volt / s) > 4.1)				  //If battery voltage is near to full charge voltage shift to CV mode 
  {
   while((cur_avg_volt / s) < 4.2)
   {	  
    wrcmd(0x01);
    wrcmd(0x80);
    lcd_print("CV Mode");
    _delay_ms(200);
    PORTD |= 0b01000000;                          //Shifting to CV mode    
    PORTD &= 0xDF;								  //Turning ON charging 
    measure_bat_voltage();						  //Keep checking current battery voltage 
    measure_charge_current();					  //Keep checking current battery charging current
    if(str_stp == 0x00)							  //If charging turned OFF through External Interrupt then come out of CC mode and turn off charging
    {
	 PORTD |= 3<<5;								  //Turning OFF charging 
	 break;	
    }
   }
   wrcmd(0x01);
   wrcmd(0x80);
   lcd_print("Battery charged ");
   str_stp = 0x00;								  //Reset the charging turn on flag to turn off state  
   s = 0;										  //Reset cell count to 0
   //start=0;
   PORTD |= 1<<5;								  //Turning OFF charging as Battery is fully charged  
  }	 
 }
}

void setup()
{
 DDRC = 0b00111100;								  //Setting LCD data pins as output pins
 DDRB = 0b00101111;								  //Setting RS and EN pins for LCD and SS=PB2, MOSI=PB3 & SCK=PB5 pins of SPI as output pins
 DDRD = 0x70;									  //Setting relay pins as output pins
 PORTC = 0b00000011;							  //Setting ADC pins as i/p pins 
 PORTD = 0x70;									  //Initializing relays' position
 PORTD |= 3<<2;  								  //Enabling Internal pull up resistor for interrupt 0 and 1 pins
 lcd_init();									  //Initialize the LCD
 lcd_print("Getting Ready");
 _delay_ms(500);
 sei();											  //Set I bit in SREG to 1 to enable global interrupts
 ADMUX = 0x40;									  //Setting AVCC with external capacitor at AREF pin with right adjusted result **REFS1  REFS0  ADLAR  –  MUX3  MUX2  MUX1  MUX0 
 ADCSRA = 0x87;									  //Enable ADC with clk rate of 125kHz **ADEN  ADSC  ADATE  ADIF  ADIE  ADPS2  ADPS1  ADPS0 
 DIDR0 = 0x03;									  //Disabling the digital inputs for ADC0 and ADC1 to reduce power consumption ** –  –  ADC5D  ADC4D  ADC3D  ADC2D  ADC1D  ADC0D 
 EIMSK = 0x03;                                    //Enable External interrupts 0 and 1 **  –  –  –  –  –  –  INT1  INT0 
 EICRA = 0x0A;									  //External interrupts 0 and 1 on falling edges ** –  –  –  –  ISC11  ISC10  ISC01  ISC00 
 SPCR = 0x52;									  //Enable SPI communication, in Master role, with clock frequency = 250kHz, in mode 0 ** SPIE  SPE  DORD  MSTR  CPOL  CPHA  SPR1  SPR0 
 SPSR = 0x00;                                     //Disable 2X speed of SPI communication ** SPIF  WCOL  –  –  –  –  –  SPI2X 
 PORTB |= 0X04;								      //Set slave select pin to logic 1 to stop SPI communication with MCP4131
}

void loop()
{
 uint16_t tmp;									  //Variable containing float values adjusted to appropriate no. of significants			
 char char_res[4]= {0};                           //Array containing float results in character format
 char th,h,t,u;									  //Digits at Thousand's, Hundredth, Tenth and Unit's place			
 tmp =cur_avg_volt * 100;						  	  
 h = ((tmp / 100) + 0x30);
 t = (((tmp % 100) / 10) + 0x30);
 u = ((((tmp % 100) % 10)) + 0x30);
 char_res[0] = h;
 char_res[1] = '.';
 char_res[2] = t;
 char_res[3] = u;	  				
 wrcmd(0xC0);
 lcd_print(char_res);
 lcd_print("V  ");
 tmp =cur_avg_curr * 1000;
 th = ((tmp / 1000) + 0x30);
 h = (((tmp % 1000) / 100) + 0x30);
 t = ((((tmp % 1000) % 100) / 10) + 0x30);
 u = ((((tmp % 1000) % 100) % 10) + 0x30);
 char_res[0] = th;
 char_res[1] = h;
 char_res[2] = t;
 char_res[3] = u;
 wrcmd(0xC8);
 lcd_print(char_res);
 lcd_print("mA  ");	
 mode_select();
   wrcmd(0x01);
   wrcmd(0x80);
   lcd_print("Battery11");
   _delay_ms(200);
}

int main(void)
{
	setup();
    while (1) 
    {
     loop();
    }
	return 0;
}