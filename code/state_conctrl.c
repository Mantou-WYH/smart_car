#include "zf_common_headfile.h"
 
car_state current_state;
car_circle_state current_circle_state;


extern int element_line_L,element_line_R,element_line_U,element_line_B,element_line_M;
extern int run_detect;

int isopen = 0;
int circle_dir = 0;

//��ʽ��{���������, �Ҳ�������, �Ϸ��������, �·��������, �ֻ���״̬ ����> ����״̬, �»���״̬}
StateTransition transitions[] = {				//Ԫ����˳�����ң��ϣ��£�
	
	{1,1,1,1,non_circle,cross,non_circle},
	
	//{0,0,0,0,non_circle,stop,non_circle},

	{0,0,2,2,non_circle,circle,in},
	
	{0,0,1,1,non_circle,straight,non_circle},	
	
	{1,0,0,1,non_circle,Lturn,non_circle},
	{0,1,0,1,non_circle,Rturn,non_circle},

	

	//{wildcard,wildcard,wildcard,wildcard,non_circle,normal,non_circle}

    // 100(wildcard)Ϊͨ���
};


//-------------------------------------------------------------------------------------------------------------------
//  �������      ��ǳ��������·״̬
//  ��������     void
//  ʹ������     isOpen();
//  ע�⣺       ����current_stateΪopen_run��isopen��־Ϊ1
//-------------------------------------------------------------------------------------------------------------------
void isOpen(){
	current_state = open_run;
	isopen = 1;	
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ��ǳ����뿪��·״̬
//  ��������     void
//  ʹ������     noOpen();
//  ע�⣺       ����current_stateΪnormal��isopen��־Ϊ0
//-------------------------------------------------------------------------------------------------------------------
void noOpen(){
	current_state = normal;
	isopen = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      �������Ƕȼ����С��ת�Ƕ�
//  ��������     int����ת�Ƕȣ�
//  ����˵��     A ��ʼ�Ƕȣ�0-359��
//              B Ŀ��Ƕȣ�0-359��
//  ʹ������     int delta = calculate_rotation_angle(30, 300);
//  ע�⣺       ����circle_dirȷ����ת����1��˳ʱ�룬2����ʱ�룩
//-------------------------------------------------------------------------------------------------------------------
int calculate_rotation_angle(int A, int B) {
    int delta;
    if (circle_dir == 2) {
        // ˳ʱ����ת�Ƕ�
        delta = (B - A) % 360;
        if (delta < 0) delta += 360;
    } else {
        // ��ʱ����ת�Ƕ�
        delta = (A - B) % 360;
        if (delta < 0) delta += 360;
    }
    return delta;
}


//-------------------------------------------------------------------------------------------------------------------
//  �������      ����Ļ��ʾ��ǰ����״̬��Ϣ
//  ��������     void
//  ʹ������     show_state();
//  ˵����       ��IPS��Ļ�ض�λ����ʾ��״̬�ͻ�����״̬
//-------------------------------------------------------------------------------------------------------------------
void show_state(){
	//ips200_show_int(0,220,isopen,2);
	 switch(current_state) {
        case straight:
            ips200_show_string(0, 240, "straight");
            break;
        case cross:
            ips200_show_string(0, 240, "cross");
            break;
        case Rcircle:
            ips200_show_string(0, 240, "Rcircle");
            break;
        case Lcircle:
            ips200_show_string(0, 240, "Lcircle");
            break;
		case circle:
            ips200_show_string(0, 240, "circle");
            break;
		case normal:
            ips200_show_string(0, 240, "normal");
            break;
		case open_run:
            ips200_show_string(0, 240, "open_run");
            break;
		case Lturn:
            ips200_show_string(0, 240, "Lturn");
            break;
		case Rturn:
            ips200_show_string(0, 240, "Rturn");
            break;
	 }

    // ���� car_circle_state ���
    switch(current_circle_state) {
        case non_circle:
            ips200_show_string(0, 260, "non_circle");
            break;
        case R_run:
            ips200_show_string(0, 260, "R_run");
            break;
        case in:
            ips200_show_string(0 , 260, "in");
            break;
		case L_in:
            ips200_show_string(0, 260, "L_in");
            break;
		case R_in:
            ips200_show_string(0, 260, "R_in");
            break;
        case L_run:
            ips200_show_string(0, 260, "L_run");
            break;
		case L_circle_out:
            ips200_show_string(0, 260, "L_circle_out");
            break;
		case R_circle_out:
            ips200_show_string(0, 260, "R_circle_out");
            break;
				
    }
}

uint8_t check(int i){
	if(transitions[i].L !=element_line_L&& transitions[i].L != wildcard) return 0;
	if(transitions[i].R !=element_line_R&& transitions[i].R!= wildcard) return 0;
	if(transitions[i].U !=element_line_U&& transitions[i].U!= wildcard) return 0;
	if(transitions[i].B !=element_line_B&& transitions[i].B != wildcard) return 0;
	if(transitions[i].lats_circle_state!= current_circle_state) return 0;
	return 1;
	
	
}



//-------------------------------------------------------------------------------------------------------------------
//  �������      ���³�����״̬�ͻ�����״̬
//  ��������     void
//  ʹ������     update_state();
//  ˵����       ����״̬��ʵ�֣����ݼ�����͵�ǰ״̬����ת����
//              1. ���״̬ת����
//              2. �����·״̬
//              3. ����ʮ��·����״̬
//              4. ���������롢���к��˳��߼�
//              5. ���ٻ�����ת�Ƕ�
//-------------------------------------------------------------------------------------------------------------------
void update_state() {
    static int circle = 0;
	static float current_angle;
	static uint8_t open_check = 0;


	
	

	if(current_state==open_run){
		//ips200_show_string(0,120,"!!");
		return;
	}
	
	if(current_state==stop){
		//ips200_show_string(0,120,"!!");
		return;
	}

	
	if(current_state == cross){
		if(element_line_B>0&&element_line_L>0&&element_line_R>0&&element_line_U>0){
			return;
		}
	}

    for (int i = 0; i < sizeof(transitions)/sizeof(transitions[0]); i++) {
        if (check(i)) {
            current_state = transitions[i].new_state;
            current_circle_state = transitions[i].new_circle_state;
            break;
		}
		
    }
		
	if(current_circle_state==in){
		//ips200_show_string(0,120,"!!");
		if(circle_r==1){
			current_state = Rcircle;
			current_circle_state = R_in;
			circle_dir = 1;
		}else if(circle_l==1){
			current_state = Lcircle;
			current_circle_state = L_in;
			circle_dir = 2;
		}else{
			current_state = normal;
			current_circle_state= non_circle;
		}
	}
	
	if(circle==0&&(current_circle_state== R_in||current_circle_state==L_in)){						
		circle = 1;
		current_angle = get_yaw();
		//ips200_show_int(0,120,(int)current_angle,3);
	}
	if(circle==1){
		gpio_set_level(LED1,1);
		//ips200_show_int(0,120,abs((int)(current_angle - get_yaw())),2);
		if(abs((int)(current_angle - get_yaw()))>90&&abs((int)(current_angle - get_yaw()))<100){
			current_state = normal;
			if(current_circle_state== R_in){
				current_circle_state= R_run;
			}else if(current_circle_state == L_in){
				current_circle_state= L_run;
			}
			circle = 0;
		}
	}else{
	gpio_set_level(LED1,0);
	}
	
	if(current_circle_state==R_run||current_circle_state==L_run){
		//ips200_show_int(0,130,calculate_rotation_angle(current_angle,get_yaw()),3);
		if(calculate_rotation_angle(current_angle,get_yaw())>=345&&calculate_rotation_angle(current_angle,get_yaw())<=346 ){
			if(current_circle_state== R_run){
				current_circle_state = R_circle_out;
			}else if(current_circle_state == L_run){
				current_circle_state = L_circle_out;
			}
			timer_start(TC_TIME2_CH0);
		}
	}
	
	if(current_circle_state==L_circle_out||current_circle_state==R_circle_out){
		//ips200_show_int(0,130,calculate_rotation_angle(current_angle,get_yaw()),3);
		if(timer_get(TC_TIME2_CH0)>1000){
			current_circle_state = non_circle;
			circle_dir = 0;
			timer_stop(TC_TIME2_CH0);
			timer_clear(TC_TIME2_CH0);
		}
	}
	
	if(current_state == open_run&&open_check==0){													//��·
		open_check = 1;
		isopen = 1;
	}

	if(open_check==1&&current_state!=open_run){
		open_check = 0;
		count_navigation++;
	}


	//show_state();

}