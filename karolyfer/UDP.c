

#include "onewire.h"
#include "ds18b20.h"
#include "UDP.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

const __flash char UDP_Set_Wi_Fi[]		= "At+CWMODE=3\r\n";
const __flash char UDP_Enable_MUX[]		= "AT+CIPMUX=1\r\n";
const __flash char UDP_Create_UDP[]		= "AT+CIPSTART=4,\"UDP\",\"192.168.43.72\",1112,8082,0\r\n";
const __flash char UDP_Send_Data[]		= "AT+CIPSEND=4,";
const __flash char UDP_Close_UDP[]		= "AT+CIPCLOSE=4\r\n";
const __flash char UDP_Return_Line[]	= "\r\n";
const __flash char UDP_Quey_IP[]		= "AT+CIFSR\r\n";
const __flash char UDP_Test_Signal[]    = "AT+CIPSEND=4,4\r\n";
const __flash char UDP_Test_Signal2[]	= "-xx-";
const __flash char UDP_Response_Test[]	= "AT+CIPSEND=4,16\r\n";
//const __flash char UDP_WiFi[]	=	"AT+CWJAP_CUR=\"NAME\",\"Password\"\r\n";

volatile char Module_Response[100];
volatile int Module_Response_Index;

char Received_Data[20];

void UDP_delay()
{
	// funkcja opozniajaca na podstawie danych otrzymanych od modulu
	unsigned int indeks_poprzedni;
	int licznik = 0;
	cli();
	do
	{
		indeks_poprzedni = Module_Response_Index;
		sei();
		_delay_ms(1);
		licznik += 1;
		cli();
	}
	while (indeks_poprzedni != Module_Response_Index  && licznik < 100);
	sei();
	_delay_ms(10);
}

/*
//Method determines what type of action is to be taken based of a response from a module
int UDP_check_response(const char *resp)
{
	if (strstr(resp, "ERROR") != NULL)
		return UDP_RESPONSE_ERROR;
	if (strstr(resp, "OK") != NULL)
		return UDP_RESPONSE_OK;
	if (strstr(resp, "+IPD") !=NULL)
		return UDP_RECEIVED_DATA;
}
*/


int UDP_check_response()
{
	if (memmem(Module_Response, Module_Response_Index+1, "ERROR", strlen("ERROR")) != NULL)
	return UDP_RESPONSE_ERROR;
	else return UDP_RESPONSE_OK;
}


//Method waiting to be finished
void UDP_send_command(const __flash char *cmd)
{
	for(int i = 0; i < strlen_P(cmd); i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = cmd[i];
	}
	UDP_delay();
}

void UDP_send_command_N(const char *cmd)
{
	for(int i = 0; i < strlen(cmd); i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = cmd[i];
	}
	UDP_delay();
}

void UDP_send_string(volatile char *string, int byte_count)
{
	for(int i = 0; i < byte_count; i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = string[i];
	}
	UDP_delay();
}

//Method sends data to another connected device
void UDP_send_command_and_data(const __flash char *cmd, char *data)
{
	//Sending first part of the command
	UDP_send_command(UDP_Send_Data);
	//Calculating size of sent data
	int size = strlen_P(data);
	char* scndPart;
	itoa(size, scndPart, 10);
	//Sending calculated size of data
	//UDP_send_string(scndPart);
	UDP_send_command(UDP_Return_Line);
	//Sending data
	//UDP_send_string(data);
}

//Setting up whole UDP communication
void UDP_start_UDP()
{
	//UDP_send_command(UDP_Set_Wi_Fi);
	UDP_send_command(UDP_Enable_MUX);
	UDP_send_command(UDP_Create_UDP);
}

//Dumb and primitive way, probably doesn't even work
int UDP_search_for(volatile char* src, char search)
{
	for(int i = 0; i < strlen(src); i++)
	{
		if(src[i] == search) return UDP_RESPONSE_OK;
	}
	return UDP_RESPONSE_ERROR;
}

void UDP_connect()
{
	UDP_send_command(UDP_WiFi);
	UDP_delay();
	_delay_ms(9000);
	UDP_delay();
}

int UDP_send_UDP(const char* msg)
{
	UDP_send_command_N("AT+CIPSEND=4,");
	char length[16];
	itoa(strlen(msg), length, 10);
	UDP_send_command_N(length);
	UDP_send_command(UDP_Return_Line);
	UDP_send_command_N(msg);
}

inline int UDP_try(const char* msg)
{
	return memmem(Module_Response, Module_Response_Index+1, msg, strlen(msg));
}