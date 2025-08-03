#ifndef _STATE_H_
#define _STATE_H_

#define wildcard 100
typedef enum {
	normal,
	straight,
	cross,
	circle,
	Rcircle,
	Lcircle,
	Lturn,
	Rturn,
	open_run,
	fold,
	stop,
}car_state;

typedef enum {
	non_circle,
	in,
	R_in,
	L_in,
	R_run,
	L_run,
	L_circle_out,
	R_circle_out,
	circle_stop,
}car_circle_state;

typedef struct {
    int L;
    int R;
    int U;
    int B;
    car_circle_state lats_circle_state;
	car_state new_state;
	car_circle_state new_circle_state;
} StateTransition;

extern car_state current_state;
extern car_circle_state current_circle_state;

void update_state();
void isOpen();
void noOpen();


#endif