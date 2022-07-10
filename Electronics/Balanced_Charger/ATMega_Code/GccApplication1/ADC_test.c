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
	char t,u;
	ADMUX = ((ADMUX & 0xF0) | (i & 0x0F) );  //Select the ADC channel voltage = 0, current = 1;
	ADCSRA |= 0x40;							 //Start conversion
	while((ADCSRA & 0x40) == 0x40){}	     //Wait still conversion is complete
	t = ADCL;
	u = ADCH;
	ADCSRA |= 0x10;							 //Clear ADIF
	tmp = (t | (u << 8));                    //10 bit ADC result
	return (tmp / 10);
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
 lcd_print("Getting Ready1");
 _delay_ms(2000);
 sei();												//Set I bit in SREG to 1 to enable global interrupts
 ADMUX |= 0x00;										//Setting AVCC with external capacitor at AREF pin with right adjusted result
 ADCSRA |= 0x87;										//**********Still to be modified for ADC clock division**********
 DIDR0 = 0x03;										//Disabling the digital inputs for ADC0 and ADC1
 EIMSK = 0x03;                                      //Enable External interrupts 0 and 1
 EICRA = 0x0A;										//External interrupts 0 and 1 on falling edges
}

void loop()
{
 uint16_t tmp_1;	
 //char buf[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 char t,u;
 tmp_1 = ADC_result(0); 
 t = ((tmp_1 / 10) + 0x30);
 u = ((tmp_1 % 10) + 0x30);
 //sprintf(buf,"%3.5f",tmp_1);
 wrcmd(0x01);
 wrcmd(0x80);
 lcd_print("ADC output1:    ");
 wrcmd(0xC0);
 wrdata(t);
 _delay_ms(2);
 wrdata(u);
 _delay_ms(100);
 //lcd_print(buf);				
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