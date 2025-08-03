#include "zf_common_headfile.h"
 
car_state current_state;
car_circle_state current_circle_state;


extern int element_line_L,element_line_R,element_line_U,element_line_B,element_line_M;
extern int run_detect;

int isopen = 0;
int circle_dir = 0;

//格式：{左侧检测数量, 右侧检测数量, 上方检测数量, 下方检测数量, 现环岛状态 ——> 新主状态, 新环岛状态}
StateTransition transitions[] = {				//元素行顺序左，右，上，下，
	
	{1,1,1,1,non_circle,cross,non_circle},
	
	//{0,0,0,0,non_circle,stop,non_circle},

	{0,0,2,2,non_circle,circle,in},
	
	{0,0,1,1,non_circle,straight,non_circle},	
	
	{1,0,0,1,non_circle,Lturn,non_circle},
	{0,1,0,1,non_circle,Rturn,non_circle},

	

	//{wildcard,wildcard,wildcard,wildcard,non_circle,normal,non_circle}

    // 100(wildcard)为通配符
};


//-------------------------------------------------------------------------------------------------------------------
//  函数简介      标记车辆进入断路状态
//  返回类型     void
//  使用样例     isOpen();
//  注意：       设置current_state为open_run，isopen标志为1
//-------------------------------------------------------------------------------------------------------------------
void isOpen(){
	current_state = open_run;
	isopen = 1;	
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      标记车辆离开断路状态
//  返回类型     void
//  使用样例     noOpen();
//  注意：       设置current_state为normal，isopen标志为0
//-------------------------------------------------------------------------------------------------------------------
void noOpen(){
	current_state = normal;
	isopen = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      计算两角度间的最小旋转角度
//  返回类型     int（旋转角度）
//  参数说明     A 起始角度（0-359）
//              B 目标角度（0-359）
//  使用样例     int delta = calculate_rotation_angle(30, 300);
//  注意：       根据circle_dir确定旋转方向（1：顺时针，2：逆时针）
//-------------------------------------------------------------------------------------------------------------------
int calculate_rotation_angle(int A, int B) {
    int delta;
    if (circle_dir == 2) {
        // 顺时针旋转角度
        delta = (B - A) % 360;
        if (delta < 0) delta += 360;
    } else {
        // 逆时针旋转角度
        delta = (A - B) % 360;
        if (delta < 0) delta += 360;
    }
    return delta;
}


//-------------------------------------------------------------------------------------------------------------------
//  函数简介      在屏幕显示当前车辆状态信息
//  返回类型     void
//  使用样例     show_state();
//  说明：       在IPS屏幕特定位置显示主状态和环岛子状态
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

    // 处理 car_circle_state 输出
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
//  函数简介      更新车辆主状态和环岛子状态
//  返回类型     void
//  使用样例     update_state();
//  说明：       核心状态机实现，根据检测结果和当前状态进行转换：
//              1. 检查状态转换表
//              2. 处理断路状态
//              3. 处理十字路特殊状态
//              4. 处理环岛进入、运行和退出逻辑
//              5. 跟踪环岛旋转角度
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
	
	if(current_state == open_run&&open_check==0){													//断路
		open_check = 1;
		isopen = 1;
	}

	if(open_check==1&&current_state!=open_run){
		open_check = 0;
		count_navigation++;
	}


	//show_state();

}