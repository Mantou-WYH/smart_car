#ifndef _NAVIGATION_H
#define _NAVIGATION_H



typedef struct{
	float yaw_start;
	float yaw_in;
	float yaw_run;
} Navigate;


extern int count_navigation;

void Navigation_Init();
void Navigation_point();
void Navigation_show();
int Navigation_error_in(int i);
int Navigation_error_run(int i);



#endif
