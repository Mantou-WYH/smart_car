#include "zf_common_headfile.h"

#define STEER_PWM_MIN -100
#define STEER_PWM_MAX 100

#define constrain(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

//速度环PID
IncPID pid_left = {11,0.5,0,0,  0,0,0}, pid_right = {11,0.5,0,0,0,0,0};

SteeringPID turnPID = {6.5, 0, 2.0};       // 转向外环：方向控制
SteeringPID inturnPID = {0.6, 0, 0.0001};     // 转向内环：陀螺仪角速度控制]

LowPassFilter pwmLeftFilter={0}; // 左轮PWM输出滤波器
LowPassFilter pwmRightFilter={0}; // 右轮PWM输出滤波器

//extern icm_param_t icm_datat;


int count = 0; 
int dir = 0;
char send_str[64];


/*
速度	kp		kd		kp		kd
200		8		1		0.5		0
250		9.8		2		0.6		0.0001		
275		6.5		2		0.6		0.0001	 
300		

*/








void Speed_Filter_init(void)
{
	// alpha值范围：0.0-1.0，值越小滤波越强，响应越慢
	// 推荐值：
	// 0.1-0.2: 强滤波，适合噪声大的环境
	// 0.3-0.5: 中等滤波，平衡响应和稳定性
	// 0.6-0.8: 弱滤波，响应快但可能不够稳定
	float pwm_filter_alpha = 0.8f;    // PWM输出滤波参数

	LowPassFilter_Init(&pwmLeftFilter, pwm_filter_alpha);       // 左轮PWM输出滤波器
	LowPassFilter_Init(&pwmRightFilter, pwm_filter_alpha);      // 右轮PWM输出滤波器
}

void Motor_Init(){
    gpio_init(DIR1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // GPIO 初始化为输出 默认上拉输出高
    gpio_init(DIR2, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM1, 17000, 0);                                                   // PWM 通道初始化频率 17KHz 占空比初始为 0
    pwm_init(PWM2, 17000, 0);                                                   // PWM 通道初始化频率 17KHz 占空比初始为 0
	PID_Init();
	Speed_Filter_init();
    
}


void L_Motor_Speed(int Speed){
  
    if(Speed>4000) Speed = 4000;
    if(Speed<0) Speed = 0;
    if(Speed>=0){
        gpio_set_level(DIR1, GPIO_HIGH);                                    // DIR输出高电平
        pwm_set_duty(PWM1, Speed);
    }

            
}



void R_Motor_Speed(int Speed){
    
    if(Speed>4000) Speed = 4000;
    if(Speed<0) Speed = 0;
    if(Speed>=0){
        gpio_set_level(DIR2, GPIO_HIGH);                                    // DIR输出高电平
        pwm_set_duty(PWM2, Speed);
    }


}







void speed_control_L(int target_speed_L) {
   // 设置目标速度
    pid_left.target = target_speed_L;

    
    // 获取编码器实际速度（需确保单位一致）
    int left_speed = Get_Encoder_L();

    
    // 增量PID计算
    int left_pwm = IncPID_Calculate(&pid_left, left_speed);

    
    // 输出限幅（根据硬件调整）
    left_pwm = constrain(left_pwm, -5000, 5000);

    //printf("L:%d \n",left_speed);
    // 驱动电机
    L_Motor_Speed(left_pwm);
	//printf("%d,%d\n",target_speed_L,left_speed);
	//ips200_show_int(30,140,left_pwm,3);
	//printf("left_speed:%d\n",left_pwm);
 
	
}

void speed_control_R(int target_speed_R) {
   // 设置目标速度
    pid_right.target = target_speed_R;

    
    // 获取编码器实际速度（需确保单位一致）
    int right_speed = Get_Encoder_R();

    
    // 增量PID计算
    int right_pwm = IncPID_Calculate(&pid_right, right_speed);

    
    // 输出限幅（根据硬件调整）
    right_pwm = constrain(right_pwm, -5000, 5000);

    //printf("R:%d \n",right_speed);
    // 驱动电机
    R_Motor_Speed(right_pwm);
	
	//printf("%d,%d\n",target_speed_R,right_speed);
	
	//ips200_show_int(30,160,right_pwm,3);

}


float steering_control(float path_deviation) {
    // 外环：路径偏差 -> 目标角速度
	//int angle_speed;
    float target_gyro = steering_pd_control(&turnPID, 0, path_deviation);
	
	//ips200_show_float(0,100,target_gyro,3,3);
    /*
	if(count<2000){
		if(dir==0){
		angle_speed = 20;
		}else{
		angle_speed = 10;
		}
		count++;
		
	}else{
		dir = (dir+1)%2;
		count = 0;
	}*/
    // 内环：陀螺仪实际角速度跟踪
    float steer_pwm = steering_pd_control(&inturnPID, target_gyro, -icm_data.gyro_z);
	
    // 转向输出限幅
    return constrain(steer_pwm, STEER_PWM_MIN, STEER_PWM_MAX);
}




void combined_control(int base_speed, float path_deviation) {
    // 1. 转向控制：获取转向偏差量
    float steer_pwm = steering_control(path_deviation);
    
    // 2. 非对称差速策略
    float speed_k = steer_pwm * 0.01f;
    int left_speed, right_speed;
	
	
    
    if(steer_pwm >= 0) { 
        // 左转：左轮小幅加速，右轮大幅减速
        left_speed = base_speed * (1 + speed_k * 0.1f);
        right_speed = base_speed * (1 - speed_k * 0.85f);
    } else { 
        // 右转：左轮大幅减速，右轮小幅加速
        left_speed = base_speed * (1 - (-speed_k) * 0.85f);
        right_speed = base_speed * (1 + (-speed_k) * 0.1f);
    }
    
	left_speed = (int16)LowPassFilter_Apply(&pwmLeftFilter, (float)left_speed);
	right_speed = (int16)LowPassFilter_Apply(&pwmRightFilter, (float)right_speed);
	
    // 3. 速度环控制
	speed_control_L(left_speed);
	speed_control_R(right_speed);
	//ips200_show_int(0,0,(int)steer_pwm,3);
	

	
}

