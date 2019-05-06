#ifndef UDP_H
#define UDP_H

#define UDP_RESPONSE_ERROR 0
#define UDP_RESPONSE_OK 1
#define UDP_RECEIVED_DATA 2

extern const __flash char UDP_Set_Wi_Fi[];
extern const __flash char UDP_Enable_MUX[];
extern const __flash char UDP_Create_UDP[];
extern const __flash char UDP_Send_Data[];
extern const __flash char UDP_Close_UDP[];
extern const __flash char UDP_Return_Line[];
extern const __flash char UDP_Test_Signal[];
extern const __flash char UDP_Test_Signal2[];
extern const __flash char UDP_Quey_IP[];
extern const __flash char UDP_Response_Test[];
extern const __flash char UDP_WiFi[];
extern volatile char Module_Response[100];
extern volatile int Module_Response_Index;
extern char Received_Data[20];

/*Function _delay stops program until receiving transmission has ended*/
void UDP_delay();
/*Archaic function, no idea why it's still there*/
int UDP_check_response();
/*Function _send_command sends flash argument command through USART protocol*/
void UDP_send_command(const __flash char *cmd);
/*Function _send_command_N sends argument command through USART protocol */
void UDP_send_command_N(const char *cmd);
/*Function _send_command_and_data allows to send commands with arguments*/
void UDP_send_command_and_data(const __flash char *cmd, char *data);
/*Function _send_string sends standard string through USART protocol*/
void UDP_send_string(volatile char *string, int byte_count);
/*
	Function _start_UDP sets up whole UDP connection
	For now, all connection informations are constans,
	Need reevaluation, current state isn't desired
*/
void UDP_start_UDP();
/*Stupid Funcion, don't wanna*/
int UDP_search_for(volatile char* src, char search);

void UDP_connect();

int UDP_send_UDP(const char* msg);

int UDP_try(const char* msg);

#endif