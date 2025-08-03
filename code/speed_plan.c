#include "zf_common_headfile.h"

int PLAN_SPEED = BASE_SPEED;

void speed_plan_update(){
	PLAN_SPEED=BASE_SPEED;
}


int speed_plan_return(){
	return PLAN_SPEED;
}