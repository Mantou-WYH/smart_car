#include "zf_common_headfile.h"
#include "math.h"
#include <stdio.h>


Side Side_M[150];
extern Side StarM,Side_L[150],Side_R[150];
extern Side Smooth_R[150],Smooth_L[150];
extern Side Max_Rx,Max_Lx;

extern int dashed;

int error=0;

Side target_L,target_R,target_M;


//-------------------------------------------------------------------------------------------------------------------
//  �������     ���� ĳ������ױ��е������ �� �е㷨�ߵļн�
//  ��������     int
//	����˵��		x��y�õ�����
//  ʹ������		get_angle��M.x,M.y��;
//---------------------------------------------------------------------------------------------------------------
int get_angle(int x,int y){
	x = (MT9V03X_W/2)-x;
	y = MT9V03X_H - y;
    int result = (int)(atan2f(x, y) * 180.0f / M_PI);
	return result;
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ����Ļ�Ͽ��ӻ���ʾ��⵽�ı߽���
//  ��������     void���޷���ֵ��
//	����˵��		l ���߽������r �Ҳ�߽����
//  ʹ������		Show_Line(get_L_point(), get_R_point());
//---------------------------------------------------------------------------------------------------------------

void Show_Line(int l,int r){
	
	ips200_show_int(200,30,l,3);
	ips200_show_int(200,50,r,3);
	for(int i=0;i<l;i++){
		if(Side_L[i].x>0 && Side_L[i].x<MT9V03X_W&&Side_L[i].y>0&&Side_L[i].y<MT9V03X_H){
			ips200_draw_point(Side_L[i].x,Side_L[i].y,RGB565_RED);
		}
		//ips200_draw_point(Side_M[i].x,Side_M[i].y,RGB565_YELLOW);
	}
	
	for(int i=0;i<r;i++){
		if(Side_R[i].x>0 && Side_R[i].x<MT9V03X_W&&Side_R[i].y>0&&Side_R[i].y<MT9V03X_H){
			ips200_draw_point(Side_R[i].x,Side_R[i].y,RGB565_BLUE);
		}
	}
	
}


//-------------------------------------------------------------------------------------------------------------------
//  �������      ����һ��߽���ƽ������
//  ��������     Side������ƽ��x,y����Ľṹ�壩
//	����˵��		sidein �߽������ָ��
//              min ��ʼ����
//              max ������������������
//  ʹ������		Side avg = average_point(Side_L, 10, 20);
//---------------------------------------------------------------------------------------------------------------
Side average_point(Side *sidein,int min,int max){
	Side sum;
	sum.x = 0;
	sum.y = 0;
	int count = 0;

	for(int i=min;i<max;i++){
		sum.x += sidein[i].x;
		sum.y += sidein[i].y;
		count +=  1;
	}

	sum.x = sum.x/count;
	sum.y = sum.y/count;
	return sum;
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ���ݳ���״̬��������ƫ����������ƫ��Ƕ�
//  ��������     void���޷���ֵ��
//  ʹ������		caculate_Middle_side();
//  ע�⣺       ����洢��ȫ�ֱ���error�У������߼�������
//              - ֱ��״̬����ʹ��U�ͱ߽�
//              - ����״̬����ʹ��U�ͱ߽��
//              - ����״̬�������ұ߽�ƽ��ֵ
//              - �������״̬��������߽�ƫ��
//---------------------------------------------------------------------------------------------------------------
void caculate_Middle_side(){
	int front_point = 105;
	
	Side R,L;
	Side M;
	//calculate_middle_line(get_L_point(),get_R_point());
/*	
	if(current_circle_state==L_circle_out||current_circle_state==R_circle_out){
		front_point = 150;
		//gpio_set_level(LED1,1);
	}else{
		//gpio_set_level(LED1,0);
	}
	*/
	if(current_state == straight){
		if(element_line_U>0){
			M = U_edge[0];
		}
	}
	if(current_circle_state==R_in&&element_line_U>1){
			M = U_edge[1];
	}else if(current_circle_state==L_in&&element_line_U>1){
			M = U_edge[0];
	}else{
		//gpio_set_level(LED1,0);
		if (get_L_point()>front_point){
			L = average_point(Side_L,front_point-5,front_point);
			
		}else if(get_L_point()>30){
			L = average_point(Side_L,get_L_point()-5,get_L_point());
		}else if(get_L_point()>20){
			L = average_point(Side_L,15,20);
		}else{
			L = Side_L[get_L_point()-1];
		}
		if (get_R_point()>front_point){
			R = average_point(Side_R,front_point-5,front_point);
		}else if(get_R_point()>30){
			R = average_point(Side_R,get_R_point()-5,get_R_point());
		}else if(get_R_point()>20){
			R = average_point(Side_R,15,20);
		}else{
			R = Side_R[get_R_point()-1];
		}
		M.x = (L.x+R.x)/2;
		M.y = (L.y+R.y)/2;
		//gpio_set_level(LED1,0);
	}

	//show_point(M);
	//ips200_draw_line((Max_Lx.x+Max_Rx.x)/2,0,(Max_Lx.x+Max_Rx.x)/2,120,RGB565_RED);
	
	int M_result = get_angle(M.x,M.y);
	int L_result = get_angle(L.x,L.y);
	int R_result = get_angle(R.x,R.y);
	
	

	

	
	//ips200_draw_line(0,Side_L[30].y,188,Side_L[30].y,RGB565_RED);
	
	if(current_state== open_run){
		error = 0;
	}else if(current_circle_state==R_run){
		error = R_result;
	}else if(current_circle_state==L_run){
		error = L_result;
	}else if(current_circle_state==L_circle_out){
		error = R_result;
	}else if(current_circle_state==R_circle_out){
		error = L_result;
	}else{
		error = M_result;
	}

	
	
}


//-------------------------------------------------------------------------------------------------------------------
//  �������      ��ȡ��ǰ������н�ƫ��Ƕ�
//  ��������     int��-180�㵽180���ƫ��Ƕȣ�
//	ʹ������		int steering_angle = return_error();
//---------------------------------------------------------------------------------------------------------------
int return_error(){
	return error;
}
