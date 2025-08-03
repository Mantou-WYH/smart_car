#include "zf_common_headfile.h"

extern car_state current_state;
extern car_circle_state current_circle_state;
int run_detect = 0;

int element_line_L=0,element_line_R=0,element_line_U=0,element_line_B=0,element_line_M=0;
int circle_l=0,circle_r=0;

Side L_edge[10],R_edge[10],U_edge[10],B_edge[10],M_edge[10];


//-------------------------------------------------------------------------------------------------------------------
//  函数简介     判断左右圆环
//  返回类型     uint8_t（检测到的跳变点数量）
//	参数说明		img 图像数据
//				x 检测的X坐标位置
//  使用样例		int count = line_circl_H(img_data, 100);
//---------------------------------------------------------------------------------------------------------------
uint8_t line_circl_H(uint8_t img[MT9V03X_H][MT9V03X_W],int x){
	int count=0;
	for(int i=30+2;i<MT9V03X_H-1;i++){
		if(safe_access_img(img,x,i-2)==0){
			if(safe_access_img(img,x,i-1)==0){
				if(safe_access_img(img,x,i)==255){
					if(safe_access_img(img,x,i+1)==255){
						if(safe_access_img(img,x,i+2)==255){
							count++;
						}
					}
				}
			}
		}
	}
	return count;
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      在指定高度（Y坐标）的水平方向检测跳变点（黑到白）
//  返回类型     uint8_t（检测到的跳变点数量或最大值）
//	参数说明		img 图像数据
//				hight 检测的Y坐标高度
//				max 当前最大值（用于比较）
//				*edge 存储检测到的边界点
//  使用样例		line_W(img, 50, 0, U_edge);
//---------------------------------------------------------------------------------------------------------------

uint8_t line_W(uint8_t img[MT9V03X_H][MT9V03X_W],int hight,int max,Side *edge){
	int count=0;
	for(int i=20;i<MT9V03X_W-20;i++){
		if(safe_access_img(img,i-2,hight)==0){
			if(safe_access_img(img,i-1,hight)==0){
				if(safe_access_img(img,i,hight)==255){
					if(safe_access_img(img,i+1,hight)==255){
						if(safe_access_img(img,i+2,hight)==255){
							count++;
						}
					}
				}
			}
		}
	}

	if(count>max){
		int count_point = 0;
		for(int i=20;i<MT9V03X_W-20;i++){
			if(safe_access_img(img,i-2,hight)==0){
				if(safe_access_img(img,i-1,hight)==0){
					if(safe_access_img(img,i,hight)==255){
						if(safe_access_img(img,i+1,hight)==255){
							if(safe_access_img(img,i+2,hight)==255){
								edge[count_point].x = i;
								edge[count_point].y = hight;
								count_point++;
							}
						}
					}
				}
			}
		}
	}
	return count>max?count:max;
}
//-------------------------------------------------------------------------------------------------------------------
//  函数简介      在指定X坐标的垂直方向检测跳变点（黑到白）
//  返回类型     uint8_t（检测到的跳变点数量）
//	参数说明		img 图像数据
//				x 检测的X坐标位置
//  使用样例		int count = line_H(img_data, 100);
//---------------------------------------------------------------------------------------------------------------
uint8_t line_H(uint8_t img[MT9V03X_H][MT9V03X_W],int x,int max,Side *edge){
	int count=0;
	for(int i=30+2;i<MT9V03X_H-1;i++){
		if(safe_access_img(img,x,i-2)==0){
			if(safe_access_img(img,x,i-1)==0){
				if(safe_access_img(img,x,i)==255){
					if(safe_access_img(img,x,i+1)==255){
						if(safe_access_img(img,x,i+2)==255){
							count++;
						}
					}
				}
			}
		}
	}
	
	if(count>max){
		int count_point = 0;
		for(int i=30+2;i<MT9V03X_H-1;i++){
			if(safe_access_img(img,x,i-2)==0){
				if(safe_access_img(img,x,i-1)==0){
					if(safe_access_img(img,x,i)==255){
						if(safe_access_img(img,x,i+1)==255){
							if(safe_access_img(img,x,i+2)==255){
								edge[count_point].x = x;
								edge[count_point].y = i;
								count_point++;
							}
						}
					}
				}
			}
		}
	}
		
	
	
	return count>max?count:max;
}

uint8_t element_L(uint8_t img[MT9V03X_H][MT9V03X_W]){
	int count = 0;
	for(int i=0;i<=5;i++){
		count = line_H(img,20+i*3,count,L_edge);
	}
	return count;
}
					
uint8_t element_R(uint8_t img[MT9V03X_H][MT9V03X_W]){
	int count = 0;
	for(int i=0;i<=5;i++){
		count = line_H(img,MT9V03X_W-20-i*3,count,R_edge);
	}
	return count;
}
				
uint8_t element_U(uint8_t img[MT9V03X_H][MT9V03X_W]){
	int count = 0;
	for(int i=0;i<=5;i++){
		count = line_W(img,20+i*5,count,U_edge);
	}
	return count;
}
			
uint8_t element_B(uint8_t img[MT9V03X_H][MT9V03X_W]){
	int count = 0;
	for(int i=0;i<=5;i++){
		count = line_W(img,MT9V03X_H-40-i*4,count,B_edge);
	}
	return count;
}

uint8_t element_M(uint8_t img[MT9V03X_H][MT9V03X_W]){
	int count = 0;
	for(int i=-5;i<=5;i++){
		count = line_H(img,MT9V03X_W/2+i*5,count,M_edge);
	}
	return count;
}

uint8_t circle_L(uint8_t img[MT9V03X_H][MT9V03X_W]){
	for(int i=0;i<=5;i++){
		if(line_circl_H(img,MT9V03X_W/2-30-i*3)>0){
			return 1;
		}
	}
	return 0;
}

uint8_t circle_R(uint8_t img[MT9V03X_H][MT9V03X_W]){
	for(int i=0;i<=5;i++){
		if(line_circl_H(img,MT9V03X_W/2+30+i*3)>0){
			return 1;
		}
	}
	return 0;
}

void show_element_line(){
	//ips200_show_int(0,140,element_line_M,2);
	ips200_show_int(0,160,element_line_L,2);
	ips200_show_int(0,180,element_line_R,2);
	ips200_show_int(0,200,element_line_U,2);
	ips200_show_int(0,220,element_line_B,2);
}

void show_element_first_point(){
	if(element_line_L>0){
		ips200_show_int(0,160,L_edge[0].y,3);
	}
	if(element_line_R>0){
		ips200_show_int(0,180,R_edge[0].y,3);
	}
}

void show_circie_line(){
	ips200_show_int(0,160,circle_l,2);
	ips200_show_int(0,180,circle_r,2);
}

void element_update(uint8_t img[MT9V03X_H][MT9V03X_W]){
	element_line_L = element_L(img);
	element_line_R = element_R(img);
	element_line_U = element_U(img);
	element_line_B = element_B(img);
	element_line_M = element_M(img);
	circle_l = circle_L(img);
	circle_r = circle_R(img);
	//show_element_line();
	//show_element_first_point();
	//show_circie_line();
}
