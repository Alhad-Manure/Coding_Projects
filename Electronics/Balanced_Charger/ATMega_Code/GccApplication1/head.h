#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
# define charge_on 5
# define charge_mode 6
//float map_volt=0.0048;
uint8_t I,V;				//Index variables
uint8_t s = 0;
char start = 0;				//Flag to indicate that batterry cell count is set 
char str_stp = 0x00;
float cur_avg_volt;
//uint8_t cur_avg_volt;		//For testing
float cur_avg_curr;
float battery_voltage;
float charge_current;
float voltage_avg[10];
float current_avg[10];
void genen();
void wrcmd(char c);
void wrdata(char c);
void lcd_init();
void lcd_print(char *ptr);
//uint16_t ADC_result(char i); //For testing
float ADC_result(char i);
float averager(uint8_t flg_1);
void measure_bat_voltage();
void measure_charge_current();
void mode_select();
void mcp_write(char cmd, char data);
//void float_to_char(uint16_t tmp, char flg_2);