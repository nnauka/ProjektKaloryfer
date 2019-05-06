/*
 * custom_ds18b20.c
 *
 * Created: 22.11.2018 18:42:56
 *  Author: Aleksander Nauka
 */ 

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/************************************************************************/
/* ****MY**** very own ds18b20 library
   Designed only with Projekt Kaloryfer in Mind
   That means that all ports are hardcoded                              */
/************************************************************************/

struct port_info {
	uint8_t input;
	uint8_t output;
	uint8_t direction;
	uint8_t mask;
	};

//Generic 1-Wire transmission start
//returns uint8_t mask which is the adress on 1-wire bus 
inline uint8_t custom_ds18b20_1WireTransmissionStart() {
	
	//This is where we're hardcoding one-wire bus pin
	uint8_t mask = (1 << PD7);
	
	PORTD |= mask;
	DDRD |= mask;
	PORTD &= ~mask;
	
	return mask;
}

//Closing the transmission
inline void custom_ds18b20_1WireTransmissionClose(uint8_t mask) {
	PORTD |= mask;
	DDRD |= mask;
}


/** custom_ds18b20_oneWireInit
* Initializes oneWire transmission - checks for slaves activity
*/
uint8_t custom_ds18b20_1WireInit() {
	
	uint8_t activity = 0;	
	uint8_t sreg = SREG;
	
	cli();
	//uint8_t mask = custom_ds18b20_1WireTransmissionStart();
	uint8_t mask = (1 << PD7);
	
	PORTD |= mask;
	DDRD |= mask;
	PORTD &= ~mask;
	
	//Waiting to release the bus
	_delay_us(600);
	
	//Switching to read
	DDRD &= ~mask;
	
	//waiting for slave to release the bus
	_delay_us(100);
	
	//reading slave's response
	activity = PIND & mask;
	
	_delay_us(200);
	
	//custom_ds18b20_1WireTransmissionClose(mask);
	PORTD |= mask;
	DDRD |= mask;
	
	_delay_us(600);

	SREG = sreg;
	return activity != 0 ? 1 : 0;
}

//Writing one bit on a bus
inline void custom_ds18b20_1WireWriteBitSlot(uint8_t bit) {
	
	uint8_t sreg = SREG;
	cli();
	
	//uint8_t mask = custom_ds18b20_1WireTransmissionStart();
	uint8_t mask = (1 << PD7);
	
	PORTD |= mask;
	DDRD |= mask;
	PORTD &= ~mask;
	
	if(bit != 0) _delay_us(8);
	else _delay_us(80);
	
	PORTD |= mask;
	
	if(bit != 0) _delay_us(80);
	else _delay_us(2);
	
	SREG = sreg;
}

void custom_ds18b20_1WireWrite(uint8_t data) {
	uint8_t sreg = SREG;
	cli();
	for (uint8_t i = 1; i != 0; i <<= 1) custom_ds18b20_1WireWriteBitSlot(data & i);
	SREG = sreg;
}

uint8_t custom_ds18b20_1WireReadBitSlot() {
	
	uint8_t sreg = SREG;
	cli();
	
	uint8_t response = 0;
	//uint8_t mask = custom_ds18b20_1WireTransmissionStart();
	uint8_t mask = (1 << PD7);
	
	PORTD |= mask;
	DDRD |= mask;
	PORTD &= ~mask;
	
	_delay_us(2);
	
	DDRD &= ~mask;
	
	_delay_us(8);
	
	response = ((PIND & mask) != 0);
	
	_delay_us(60);
	
	SREG = sreg;
	return response;
}

uint8_t custom_ds18b20_1WireRead() {
	uint8_t response = 0;
	uint8_t sreg = SREG;
	cli();
	for(uint8_t i = 1; i != 0; i <<= 1) response |= custom_ds18b20_1WireReadBitSlot() * i;
	SREG = sreg;
	return response;
}

void custom_ds18b20_Convert() {
	//Skip ROM Check
	custom_ds18b20_1WireInit();
	custom_ds18b20_1WireWrite(0xCC);
	//Issue Convert command
	custom_ds18b20_1WireWrite(0x44);
}

int custom_ds18b20_readTemperature() {
	uint8_t scratchpad[2];
	//Skip Rom Check
	custom_ds18b20_1WireInit();
	custom_ds18b20_1WireWrite(0xCC);
	//Issue Read Scratchpad Command
	custom_ds18b20_1WireWrite(0xBE);
	//Read bytes data from scratchpad
	for(int i = 0; i < 2; i++) scratchpad[i] = custom_ds18b20_1WireRead();
	return (int16_t)( scratchpad[1] << 8 ) + ( scratchpad[0] & 0xFF );
}

int custom_ds18b20_getTemperature() {
	custom_ds18b20_Convert();
	_delay_ms(1000);
	return custom_ds18b20_readTemperature()/16;
}