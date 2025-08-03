#include "zf_common_headfile.h"

Navigate Navigation[3];

int count_memory = 0;			//记录第几个点
int count_navigation = 0;		//调用第几个点
int count_order = 0;				//开始还是结束

int get_angle_difference(float start,float current){
	float tmp = current-start;
	if(tmp>180) tmp -= 360;
	if(tmp<-180) tmp += 360;
	return (int)tmp;
}



void Navigation_reset(){
	for(int i=0;i<3;i++){
		Navigation[i].yaw_in = 0;
		Navigation[i].yaw_run = 0;
	}
	count_order = 0;
	count_navigation = 0;


}

void Navigation_point(){
	flash_erase_page(0, count_memory);
	flash_union_buffer[0].float_type = get_yaw();
	flash_write_page_from_buffer(0, count_memory, 1);
	flash_buffer_clear();
	count_memory = (count_memory+1)%3;
}

void Navigation_show(){
	for(int i=0;i<3;i++){
		ips200_show_float(80,i*20,Navigation[i].yaw_run,3,3);
	}
}

int Navigation_error_in(int i){
	return get_angle_difference(get_yaw(),Navigation[i].yaw_in);
}

int Navigation_error_run(int i){
	flash_read_page_to_buffer(0, i, 1);
	Navigation[i].yaw_run = flash_union_buffer[0].float_type;
	flash_buffer_clear();
	return get_angle_difference(get_yaw(),Navigation[i].yaw_run);
}