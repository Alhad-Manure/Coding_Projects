/*
 * GccApplication1.c
 *
 * Created: 11-Apr-19 8:41:04 AM
 * Author : ALHAD MANURE
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "head.h"

ISR(INT0_vect)
{
 s++;
 if(s > 3)
 {
  s = 1;	 
 }
 //*****Here write your SPI function 	
 //*****Still to write function conatining write to SPI port*******
}
ISR(INT1_vect)
{
 str_stp = ~str_stp; // Start or Stop charging
 if(str_stp == 1)
 {
  wrcmd(0x01);
  wrcmd(0x80);	 
  lcd_print("Charging On");	 
 }
 else
 {
  wrcmd(0x01);
  wrcmd(0x80);
  lcd_print("Charging OFF"); 
 }
 EIFR |= 1<<INTF1;  	
}			
void genen()
{
 PORTB &= 0b11111101;
 _delay_ms(1);
 PORTB |= 0b00000010;
 _delay_ms(1);
 PORTB &= 0b11111101;
}

void wrcmd(char c)
{
 PORTB &= 0b11111110;
 PORTC = (((c & 0xf0)>>2) | 0x03);
 genen();
 PORTC = (((c & 0x0f)<<2) | 0x03);
 genen();
 _delay_ms(2);	
}

void wrdata(char c)
{
 PORTB |= 0b00000001;
 PORTC = (((c & 0xf0)>>2) | 0x03);
 genen();
 PORTC = (((c & 0x0f)<<2) | 0x03);
 genen();
 _delay_ms(2);
}

void lcd_init()
{
 _delay_ms(15);
 wrcmd(0x3F);	//Function set command
 _delay_ms(5);
 wrcmd(0x3F);	//Function set command
 _delay_ms(1);
 wrcmd(0x3F);	//Function set command
 wrcmd(0x02);	//4 bit lcd interface
 wrcmd(0x28);	//4 bit lcd interface with 2 rows 
 wrcmd(0x08);	//display off command
 wrcmd(0x01);	//clear display command
 wrcmd(0x06);    
 wrcmd(0x0c);	//display on command
 wrcmd(0x80);	//set cursor to row 0 and column 0
}

void lcd_print(char *ptr)
{
 while(*ptr)
 {
  wrdata(*ptr);
  ptr++;	 
 }
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
 if(str_stp == 0)
 {
  wrcmd(0x01);
  wrcmd(0x80);
  lcd_print("Menu:");
  wrcmd(0xc0);
  lcd_print("Start: Button1  ");
  _delay_ms(20); 
 }
  	
 else if(str_stp == 1)
 {	 
  if((cur_avg_volt / s) <= 3)
  {
   wrcmd(0x01);
   wrcmd(0x80);	  
   lcd_print("Damaged Battery ");
   _delay_ms(20);
   return;  
  }
  
  else if((cur_avg_volt / s) < 4.1)
  {
   while((cur_avg_volt / s) < 4.1)
   {  
    wrcmd(0x01);
    wrcmd(0x80);
    lcd_print("CC Mode");
    _delay_ms(20);
    PORTD |= 1<<5;                          // Turning on charging in CC mode
	measure_bat_voltage();
	measure_charge_current();
   }
  }
  
  else if((cur_avg_volt / s) > 4.1)
  {
   while((cur_avg_volt / s) < 4.2)
   {	  
    wrcmd(0x01);
    wrcmd(0x80);
    lcd_print("CV Mode");
    _delay_ms(20);
    PORTD |= 3<<5;                          //Turning ON charging and shifting to CV mode
    measure_bat_voltage();
    measure_charge_current();
   }
   PORTD ^= 3<<5;							//Turning OFF charging  
  }	 
 }
}

void setup()
{
 DDRC = 0b00111100;									//Setting LCD data pins as output pins
 DDRB = 0b00000011;									//Setting RS and EN pins of LCD as output pins
 DDRD = 0x70;										//Setting relay pins
 PORTC = 0b00000011;								//Setting ADC pins as i/p pins 
 PORTD = 0x70;										//Initializing relays' position
 PORTD |= 3<<2;  									//Enabling Internal pull up resistor for interrupt 0 and 1 pins
 lcd_init();
 lcd_print("Getting Ready");
 _delay_ms(1000);
 sei();												//Set I bit in SREG to 1 to enable global interrupts
 ADMUX = 0x40;										//Setting AVCC with external capacitor at AREF pin with right adjusted result
 ADCSRA = 0x87;										//**********Still to be modified for ADC clock division**********
 DIDR0 = 0x03;										//Disabling the digital inputs for ADC0 and ADC1
 EIMSK = 0x03;                                      //Enable External interrupts 0 and 1
 EICRA = 0x0A;										//External interrupts 0 and 1 on falling edges
}

void loop()
{
 char buf[20];
 measure_bat_voltage();
 sprintf(buf,"%d",cur_avg_volt);
 wrcmd(0x01);
 wrcmd(0x80);
 lcd_print("ADC output:     ");
 wrcmd(0xC0);
 lcd_print(buf);
 //above lines are for testing of simulation only				
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