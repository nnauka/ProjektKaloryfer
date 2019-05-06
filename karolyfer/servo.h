
#ifndef SERVO_H_
#define SERVO_H_

#define Kali_Szybko_Lewo -2 
#define Kali_Lewo -1
#define Kali_Stoj 0
#define Kali_Prawo 1
#define Kali_Szybko_Prawo 2

#define R_FAST 65
#define R_SLOW 47 
#define S_STOP 42
#define L_FAST 23
#define L_SLOW 38

#define SERVO_FAST_LEFT (OCR1A = L_FAST)
#define SERVO_FAST_RIGHT (OCR1A = L_FAST)
#define SERVO_STOP (OCR1A = S_STOP)

//dangerous macro ahead, usage is mostly forbidden
//X is not controlled by a program in any way, what may lead to unexpected behaviour, usage of function recommended
#define SERVO_START(X) OCR1A = X

void servo_Configure_Servo();
void servo_Kali_Pchaj(int Kali_co);
void servo_Rotate_By(int direction, int interval);
void servo_delay(int interval);
#endif /* SERVO_H_ */