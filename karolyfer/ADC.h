/*
 * ADC.h
 *
 * Created: 2018/03/23 17:35:21
 *  Author: Dondu
 */ 


#ifndef ADC_H_
#define ADC_H_

#define ADC_READ_CURRENT (ADCSRA |= (1 << ADSC))

extern volatile long avg;
extern volatile int Check;

void ADC_Configuration();
void ADC_timer0_configuration();
int ADC_Read_Current();



#endif /* ADC_H_ */