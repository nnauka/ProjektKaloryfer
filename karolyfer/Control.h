#ifndef CONTROL_H_
#define CONTROL_H_

#define BUSY_ROTATE	17
#define BUSY_TEMP	21 
#define ERROR		44
#define IDLE		1
#define BUSY		0

#define LEFT 0
#define STOP 1
#define RIGHT 2
#define READ 3
#define STATUS 4

#define SET0 0
#define SET1 1
#define SET2 2
#define SET3 3
#define SET4 4
#define SET5 5
#define READTEMP 6
#define READSTATUS 7
#define HELP 8
#define LEFT_TEST 9
#define RIGHT_TEST 10
#define STOP_TEST 11
#define UNDEFINED 44


//That should return timing for servo to set precise level of kaloryfer
#define WAITING_TIME(X) X*time_for_1degree

extern const char Control_Left[];
extern const char Control_Right[];
extern const char Control_Stop[];
extern const char Control_Radiator[];
extern const char Control_Temperature[];
extern const char Control_Help[];
extern const char Control_Rotate[];

extern int Control_State;

void Control_issue_command();
void Control_receive_and_execute();
//Controls command is supposed to be more efficient, need online testing
void Control_command();

int Control_handle_command();
#endif /* CONTROL_H_ */