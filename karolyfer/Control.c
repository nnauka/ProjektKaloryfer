
#include "Control.h"
#include "onewire.h"
#include "ds18b20.h"
#include "UDP.h"
#include "servo.h"
#include "ADC.h"
#include "ds18b20.h"
#include "custom_ds18b20.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

const char Control_Left[]			= "Command:Left";
const char Control_Right[]			= "Command:Right";
const char Control_Stop[]			= "Command:Stop";
const char Control_MAX[]			= "Command:MAX";
const char Control_Set5[]			= "Command:Set5";
const char Control_Set4[]			= "Command:Set4";
const char Control_Set3[]			= "Command:Set3";
const char Control_Set2[]			= "Command:Set2";
const char Control_Set1[]			= "Command:Set1";
const char Control_Set0[]			= "Command:Set0";
const char Control_Temperature[]	= "Command:Temperature";
const char Control_Status[]			= "Command:Status";
const char Control_Help[]			= "Command:Help";
const char Control_Current_Prefix[]	= "Command:";
const int time_for_1degree			= 100;
const char Control_Rotate[]	= "Command:Rotate";

int Control_State = IDLE;
int current = 0;

volatile int temperature = 0;

/**  TO DO:
*	Pomiary pradu jakos dzialajo,
*	Cza sterowac i kalibrowac serwo
*	Funkcje control zamienic na taka ktora obraca kaloryfer max lewo/ max prawo
*/
 
void Control_issue_command()
{
	//char buffer[32];
	if(Control_State)
	{
		if (UDP_try(Control_Left)) 
		{
			if(1)
			{
			UDP_send_UDP("Counter clock-wise direction initiated \n");
			servo_Kali_Pchaj(Kali_Szybko_Lewo);
			current = LEFT;
			//servo_Rotate_By(Kali_Szybko_Lewo, 500);
			//current = STOP;
			return;	
			}
			else UDP_send_UDP("Servo is already moving in that direction!\n");
		}
		else if (UDP_try(Control_Right)) 
		{
			if(1)
			{
				UDP_send_UDP("Clock-wise direction initiated \n"); 
				servo_Kali_Pchaj(Kali_Szybko_Prawo);
				current = RIGHT;
				//servo_Rotate_By(Kali_Szybko_Prawo, 500);
				//current = STOP;
				return;
			}
			else UDP_send_UDP("Servo is already moving in that direction!\n");
		}
		else if (UDP_try(Control_Stop)) 
		{
			if(1)
			{
				UDP_send_UDP("Servo has been stopped \n"); 
				//servo_Kali_Pchaj(Kali_Stoj);
				servo_Rotate_By(Kali_Stoj, 500);
				current = STOP;
				return;
			}
			else UDP_send_UDP("Servo isn't moving\n");
		}
		else if(UDP_try(Control_Temperature))
		{
			UDP_send_UDP("Measured Temperature: ");
			temperature = ds18b20control();
			char buffer[10];
			itoa(temperature, buffer, 10);
			UDP_send_UDP(buffer);
		}
		else if (UDP_try(Control_Help)) 
		{
			UDP_send_UDP("Left -> issuing Counter clock-wise movement \n, Right -> issuing clock-wise movement \n, Stop -> issuing stop command \n, Help -> Help menu \n"); 
		}
		else 
		{
			UDP_send_UDP("Unrecognized command! \n");
		}
		UDP_delay();
	}	
}

int Control_receive_request();

void Control_receive_and_execute() {
	int temp = 0;
	int request = Control_receive_request();
	char* base = "Kaloryfer set for ";
	char* base2 = "Temperature read is ";
	char* msg = "0";
	switch (request)
	{
		case HELP:
		UDP_send_UDP("Left -> issuing Counter clock-wise movement \n, Right -> issuing clock-wise movement \n, Stop -> issuing stop command \n, Help -> Help menu \n"); 
		UDP_delay();
		break;
		case SET0 ... SET5:
			itoa(request, msg, 10);
			Control_test_set_on(request);
			UDP_send_UDP(msg);
			UDP_delay();
			break;
		case READTEMP:
			custom_ds18b20_1WireInit();
			temp = custom_ds18b20_getTemperature();
			itoa(temp, msg, 10);
			UDP_send_UDP(msg);
			UDP_delay();
			break;
		case LEFT_TEST:
			servo_Kali_Pchaj(Kali_Szybko_Lewo);
			UDP_send_UDP("Kali pcha servo w lewo");
			UDP_delay();
			break;
		case RIGHT_TEST:
			servo_Kali_Pchaj(Kali_Szybko_Prawo);
			UDP_send_UDP("Kali pcha servo w prawo");
			UDP_delay();
			break;
		case STOP_TEST:
			servo_Kali_Pchaj(Kali_Stoj);
			UDP_send_UDP("Kali juz nie pcha");
			UDP_delay();
			break;
		case READSTATUS:
			break;
		default:
			UDP_send_UDP("Excuse me, what the fuck!");
			UDP_delay();
			break;
	}
}

int Control_receive_request() {
	int request = UNDEFINED;
	if(UDP_try(Control_Current_Prefix)) 
	{
		if(UDP_try(Control_Help)) request = HELP;
		else if(UDP_try(Control_Set0)) request = SET0;
		else if(UDP_try(Control_Set1)) request = SET1;
		else if(UDP_try(Control_Set2)) request = SET2;
		else if(UDP_try(Control_Set3)) request = SET3;
		else if(UDP_try(Control_Set4)) request = SET4;
		else if(UDP_try(Control_Set5)) request = SET5;
		else if(UDP_try(Control_Temperature)) request = READTEMP;
		else if(UDP_try(Control_Status)) request = READSTATUS;
		else if(UDP_try(Control_Left)) request = LEFT_TEST;
		else if(UDP_try(Control_Right)) request = RIGHT_TEST;
		else if(UDP_try(Control_Stop)) request = STOP_TEST;
	}
	return request;
}

void Control_delay(int ms) {
	int dt = 0;
	do {
		_delay_ms(1);
		dt++;
	} while(dt < ms && Check==0);
	if(Check != 0) Check = 0;
}

void Control_wait_for_end() {
	while(Check == 0);
	_delay_ms(900);
	Check = 0;
	return;
}

void Control_test_set_on(int state) {
	servo_Kali_Pchaj(Kali_Szybko_Lewo);
	Control_wait_for_end();
	//Control_delay(900);
	servo_Kali_Pchaj(Kali_Szybko_Prawo);
	for(int i = 0; i < state; i++)
	_delay_ms(205);
	servo_Kali_Pchaj(Kali_Stoj);
}

void Control_command()
{
	if(Control_State)
	{
		char* issue = memmem((const void *)Module_Response, Module_Response_Index+1, Control_Current_Prefix, strlen(Control_Current_Prefix));
		if(issue == Control_Left)
		{
			if(current != LEFT)
			{
				UDP_send_UDP("Counter clock-wise direction initiated \n");
				UDP_delay();
				//servo_Kali_Pchaj(Kali_Szybko_Lewo);
				servo_Rotate_By(Kali_Szybko_Lewo, 500);
				current = LEFT;
				return;
			}
			else UDP_send_UDP("Servo is already moving in that direction!\n");
		}
		else if(issue == Control_Right)
		{
			if(current != RIGHT)
			{
				UDP_send_UDP("Clock-wise direction initiated \n");
				UDP_delay();
				//servo_Kali_Pchaj(Kali_Szybko_Prawo);
				servo_Rotate_By(Kali_Szybko_Prawo, 500);
				current = RIGHT;
				return;
			}
			else UDP_send_UDP("Servo is already moving in that direction!\n");
		}
		else if(issue == Control_Stop)
		{
			if(current != STOP)
			{
				UDP_send_UDP("Servo has been stopped \n");
				UDP_delay();
				//servo_Kali_Pchaj(Kali_Stoj);
				servo_Rotate_By(Kali_Stoj, 500);
				current = STOP;
				return;
			}
			else UDP_send_UDP("Servo isn't moving\n");
		}
		else if(issue == Control_Temperature)
		{
			UDP_send_UDP("Measured Temperature: ");
			UDP_delay();
			temperature = ds18b20control();
			UDP_send_UDP((char*)temperature);
		}
		else if(issue == Control_Help)
		{
			UDP_send_UDP("Left -> issuing Counter clock-wise movement \n, Right -> issuing clock-wise movement \n, Stop -> issuing stop command \n, Help -> Help menu \n");
			UDP_delay();
		}
		UDP_delay();
	}
}
