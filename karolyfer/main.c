
#include "Control.h"
#include "onewire.h"
#include "ds18b20.h"
#include "UDP.h"
#include "servo.h"
#include "ADC.h"
#include "custom_ds18b20.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

//Debug option, DEBUG 1 will run program in debug mode, printing ADC registers values
#define DEBUG 0

//volatile int counter_test = 1;
volatile int blink = 0;
volatile unsigned int current_current = 0;
// @param weight - the weight of the latest adc read, actual weight is equal to (1/(weight-1))
volatile const int weight = 100;
// @param borderValue - indicates maximum read value that doesn't stop servo
volatile const int borderValue = 7500;


//ustawienie usart pod modul Wi-Fi
void usart_inicjuj_115200(void)
{
	UBRRL =	7;
	UCSRA |= (1 << U2X);
	UCSRB |= (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
	DDRD |= (1 << PD1);
}


ISR(USART_RXC_vect)
{
	Module_Response[Module_Response_Index] = UDR;
	if(Module_Response_Index < sizeof(Module_Response)-1)
	Module_Response_Index++;
}

void clearResponse() {
	for(int i = 0; i < sizeof(Module_Response_Index)-1; i++)
	Module_Response[i] = 0;
}

ISR(TIMER0_OVF_vect)
{
	ADC_READ_CURRENT;
}

	
ISR(ADC_vect)
{
	//ADLAR = 1, discarding 2 least important bits
	//Actually reading register correctly
	current_current = ADC;
	if(current_current != 0)
	avg = (avg*(weight-1)+current_current)/weight;
	if(avg >= borderValue)
	{
		OCR1A = S_STOP;
		Control_State = IDLE;
		Check = 1;
		//if(!DEBUG) {avg = 0;}
	}
	//else if(Control_State == BUSY) ADC_READ_CURRENT;
	//ADC_READ_CURRENT;
}

void blink_diode()
{
	while(blink)
	{
		PORTD |= (1 << PD3);
		
		_delay_ms(100);
		
		PORTD &= ~(1 << PD3);
		
		_delay_ms(100);
		
		blink--;
	}
}


//Main for Projekt Kaloryfer

int main(void)
{
	servo_Configure_Servo();
	servo_Kali_Pchaj(Kali_Stoj);
	DDRD |= (1 << PD3);
	usart_inicjuj_115200();
	_delay_ms(2000);
	sei();
	//command(komenda_RST);
	
	if(!DEBUG)
	{
		UDP_connect();
		UDP_send_command(UDP_Close_UDP);
		UDP_start_UDP();
		_delay_ms(1000);
		UDP_send_command(UDP_Quey_IP);
		UDP_send_UDP("Controller ready for instructions...  \n");
		UDP_delay();
		UDP_send_UDP("Left  -> issuing counter clock-wise movement,\nRight -> issuing clock-wise movement,\nStop  -> issuing stop command, \nHelp  -> Help menu \n");
		UDP_delay();
	}
	else
	{
		UDP_send_command(UDP_Close_UDP);
		UDP_start_UDP();
		_delay_ms(1000);
		UDP_send_command(UDP_Quey_IP);
	}
	
	ADC_Configuration();
	ADC_timer0_configuration();
	//ADC_Read_Current();
	
	blink = 5;
	blink_diode();
	
	if(!DEBUG)
	{
		while(1)
		{
			Module_Response_Index = 0;
			int starting_index = Module_Response_Index;
			//awaiting for a received command by waiting for Module Response Index to increment
			while(starting_index == Module_Response_Index) continue;
			UDP_delay();
			//cli();
			//Control_issue_command();
			Control_receive_and_execute();
			_delay_ms(1000);
			avg = 0;
			clearResponse();
			//sei();
		}
	} else
	{
		//ADC_Configuration();
		char buffer[10];
		//_delay_ms(75);
		ADC_Read_Current();
		
		if(1){
			int max = 0;
			int i = 500;
			_delay_ms(500);
			while(i)
			{
				cli();
				if(i == 500) servo_Kali_Pchaj(Kali_Szybko_Prawo);
				itoa(avg, buffer, 10);
				UDP_send_string(buffer, strlen(buffer));
				UDP_send_command(UDP_Return_Line);
				if(avg > max) max = avg;
				i--;
				sei();
			}
			itoa(max, buffer,10);
			UDP_send_string(buffer, strlen(buffer));
			UDP_send_command(UDP_Return_Line);
			UDP_delay();
			servo_Kali_Pchaj(Kali_Stoj);
		}
		
	}
}


//Main for custom_ds18b20 librarby testing
/*
int main(void) {
	DDRD |= (1 << PD3);
	blink = custom_ds18b20_1WireInit();
	blink_diode();
	blink = custom_ds18b20_getTemperature();
	blink_diode();
	while(1);
}
*/