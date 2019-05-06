#include "servo.h"
#include "ADC.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

void servo_Configure_Servo()
{
	DDRB |= (1 << PB1);  
	TCCR1A |= (1 << COM1A1) | (1 << WGM11);
	TCCR1A &= ~((1 << WGM10) | (1 << COM1A0));
	
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS12);
	TCCR1B &= ~((1 << CS11) | (1 << CS10));
	 
	//Overflow interrupt enable
	//TIMSK |= (1 << TOIE1);
	 
	ICR1 = 575;
}

void servo_Kali_Pchaj(int Kali_co)
{
	switch(Kali_co)
	{
		case Kali_Szybko_Lewo:
			OCR1A = L_FAST;
			break;
			
		case Kali_Lewo:
			OCR1A = L_SLOW;
			break;
		
		case  Kali_Stoj:
			OCR1A = S_STOP;
			break;
		
		case Kali_Prawo:
			OCR1A = R_SLOW;
			break;
		
		case Kali_Szybko_Prawo:
			OCR1A = R_FAST;
			break;
		
		default:
			OCR1A = S_STOP;
			break;
	}
}

void servo_delay(int interval)
{
	while(interval)
	{
		_delay_ms(1);
		interval--;
	}
}

void servo_Rotate_By(int direction, int interval)
{
	switch(direction)
	{
		case Kali_Szybko_Lewo:
		OCR1A = L_FAST;
		servo_delay(interval);
		OCR1A = S_STOP;
		break;
		
		case Kali_Lewo:
		OCR1A = L_SLOW;
		servo_delay(interval);
		OCR1A = S_STOP;
		break;
		
		case  Kali_Stoj:
		OCR1A = S_STOP;
		break;
		
		case Kali_Prawo:
		OCR1A = R_SLOW;
		servo_delay(interval);
		OCR1A = S_STOP;
		break;
		
		case Kali_Szybko_Prawo:
		OCR1A = R_FAST;
		servo_delay(interval);
		OCR1A = S_STOP;
		break;
		
		default:
		OCR1A = S_STOP;
		break;
	}
}



