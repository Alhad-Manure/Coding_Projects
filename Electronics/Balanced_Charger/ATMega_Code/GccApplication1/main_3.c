/*
 * GccApplication1.c
 *
 * Created: 11-Apr-19 8:41:04 AM
 * Author : ALHAD MANURE
 */ 
#include "head.h"

ISR(INT0_vect)
{ 
 uint8_t i;	   	
 if(str_stp == 0xFF)
 {
  wrcmd(0x01);
  wrcmd(0x80);	 
  lcd_print("First stop the  ");
  wrcmd(0xC0);
  lcd_print("Charging");
  _delay_ms(500);
 }
 else
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
	 mcp_write(0x11, 66);
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
	measure_bat_voltage();   
   }
   start=1;
 }
 EIFR |= 1<<INTF0;					//Clear the External interrupt 0 flag
}
ISR(INT1_vect)
{
 str_stp ^= 0xFF;  // Start or Stop charging
 wrcmd(0x01);
 wrcmd(0x80);
 lcd_print("str_stp: ");
 wrdata(str_stp);
 _delay_ms(100);
 if(str_stp == 0xFF)
 {
  wrcmd(0x01);
  wrcmd(0x80); 
  if(start == 1)
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
 else
 {
  wrcmd(0x01);
  wrcmd(0x80);
  lcd_print("ye kaha aa gayes");	 
 }
 start=0;
 EIFR |= 1<<INTF1;							//Clear the External interrupt 1 flag
 _delay_ms(500);  	
}			

void mcp_write(char cmd, char data)
{	
 PORTB &= 0XFB;								//Set slave select pin to logic 0 to start SPI communication with MCP4131
 wrcmd(0xC0);
 lcd_print("In MCP          ");
 _delay_ms(100);
 SPDR = cmd;								//Send Upper 8-bits of 16 bit command
 while((SPSR & 0x80) != 0x80){}				//wait till transmission is complete
 SPDR = data;			     				//Send Lower 8-bits of 16 bit command
 while((SPSR & 0x80) != 0x80){}				//wait till transmission is complete
 PORTB |= 0X04;								//Set slave select pin to logic 1 to stop SPI communication with MCP4131	 
}

void genen()								//Function to toggle LCD Enable pin
{
 PORTB &= 0b11111101;
 _delay_ms(1);
 PORTB |= 0b00000010;
 _delay_ms(1);
 PORTB &= 0b11111101;
}

void wrcmd(char c)							//Function to write command to LCD
{
 PORTB &= 0b11111110;						//Set LCD RS pin to logic 0
 PORTC = (((c & 0xf0)>>2) | 0x03);
 genen();
 PORTC = (((c & 0x0f)<<2) | 0x03);
 genen();
 _delay_ms(2);	
}

void wrdata(char c)							//Function to write data to LCD
{
 PORTB |= 0b00000001;						//Set LCD RS pin to logic 1
 PORTC = (((c & 0xf0)>>2) | 0x03);
 genen();
 PORTC = (((c & 0x0f)<<2) | 0x03);
 genen();
 _delay_ms(2);
}

void lcd_print(char *ptr)
{
 while(*ptr)
 {
  wrdata(*ptr);
  ptr++;
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
 wrcmd(0x06);    
 wrcmd(0x0c);								//display on command
 wrcmd(0x80);								//set cursor to row 0 and column 0
}

float ADC_result(char i)
{
 float tmp;
 ADMUX = ((ADMUX & 0xF0) | (i & 0x0F));  //Select the ADC channel voltage = 0, current = 1;
 ADCSRA |= 0x40;						 //Start conversion
 while((ADCSRA & 0x10)!=0x10){}			 //Wait still conversion is complete
 ADCSRA |= 0x10;						 //Clear ADIF 
 tmp = (ADCL | (ADCH << 8));             //10 bit ADC result
 return tmp;
}

float averager(uint8_t flg_1)
{
 uint8_t j;
 float res=0;	
 if(flg_1 == 1)
 {
  current_avg[I] = charge_current;
  I++;
  if(I == 10)
  {
   I = 0;
  }
  for(j=0;j<10;j++)
  {
   res += current_avg[j]; 
  } 
 }
 else if(flg_1 == 2)
 {
  voltage_avg[V] = battery_voltage;
  V++;
  if(V == 10)
  {
   V = 0;
  }
  for(j=0;j<10;j++)
  {
   res += voltage_avg[j];	  
  } 	 
 }
 res = (res / 10);
 return res;
} 

void measure_bat_voltage()
{
 battery_voltage = ADC_result(0);
 battery_voltage = s * 0.0048 * battery_voltage;    //*****Set this S value in External interrupt 1*****
 cur_avg_volt = averager(2);						//Average of last ten battery voltage readings
 _delay_ms(50); 
}

void measure_charge_current()
{
 charge_current = ADC_result(1);
 charge_current = 0.0269 * (charge_current - 512);	//Converting analog output of ACS712-5B to actual current value
 cur_avg_curr = averager(1);						//Average of last ten charging current readings 
}

void mode_select()
{
 if(str_stp == 0x00)
 {
  wrcmd(0x01);
  wrcmd(0x80);
  lcd_print("Menu-");
  lcd_print(" Mode: key1");
  wrcmd(0xC5);
  lcd_print("Start: key2");
  _delay_ms(20); 
 }  	
 else if(str_stp == 0xFF && start == 0)				//Checking Battery status
 {
  wrcmd(0x01);
  wrcmd(0x80);	 	 	
  if((cur_avg_volt / s) <= 1)
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
    _delay_ms(20);
    PORTD &= 0x9F;								   //Turning on charging in CC mode 
    measure_bat_voltage();
    measure_charge_current();
    if(str_stp == 0x00)
    {
	 PORTD |= 3<<5;								  //Turning OFF charging 
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
    _delay_ms(20);
    PORTD |= 0b01000000;                          //Shifting to CV mode    
    PORTD &= 0xDF;								  //Turning ON charging 
    measure_bat_voltage();						  //Keep checking current battery voltage 
    measure_charge_current();					  //Keep checking current battery charging current
    if(str_stp == 0x00)
    {
	 PORTD |= 3<<5;								  //Turning OFF charging 
	 break;	
    }
   }
   wrcmd(0x01);
   wrcmd(0x80);
   lcd_print("Battery charged ");
   str_stp = 0x00;
   s = 0;
   PORTD |= 1<<5;								  //Turning OFF charging  
  }	 
 }
}

void setup()
{
 DDRC = 0b00111100;								  //Setting LCD data pins as output pins
 DDRB = 0b00101111;								  //Setting RS and EN pins for LCD and SS=PB2, MOSI=PB3 & SCK=PB5 pins of SPI as output pins
 DDRD = 0x70;									  //Setting relay pins
 PORTC = 0b00000011;							  //Setting ADC pins as i/p pins 
 PORTD = 0x70;									  //Initializing relays' position
 PORTD |= 3<<2;  								  //Enabling Internal pull up resistor for interrupt 0 and 1 pins
 lcd_init();
 lcd_print("Getting Ready");
 _delay_ms(500);
 sei();											  //Set I bit in SREG to 1 to enable global interrupts
 ADMUX = 0x40;									  //Setting AVCC with external capacitor at AREF pin with right adjusted result
 ADCSRA = 0x87;									  //**********Still to be modified for ADC clock division**********
 DIDR0 = 0x03;									  //Disabling the digital inputs for ADC0 and ADC1
 EIMSK = 0x03;                                    //Enable External interrupts 0 and 1
 EICRA = 0x0A;									  //External interrupts 0 and 1 on falling edges
 SPCR = 0x52;									  //Enable SPI communication in Master mode with clock frequency = 250kHz
 SPSR = 0x00;                                     //Enable 2X speed of SPI communication
 //mcp_write(0x40, 0x03);						  //Initialize the Digital Potentiometer so that P0B and P0W are connected to Resistor 0 network
 PORTB |= 0X04;								      //Set slave select pin to logic 1 to stop SPI communication with MCP4131
 wrcmd(0x01);
 wrcmd(0x80);
 lcd_print("Start");                                     
 mcp_write(0x11, 0xFF);
 wrcmd(0x01);
 wrcmd(0x80);
 lcd_print("Success");
}

void loop()
{				
 mode_select();
}

int main(void)
{
	setup();
    /* Replace with your application code */
    while (1) 
    {
		loop();
    }
	return 0;
}