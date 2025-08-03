#include "zf_common_headfile.h"

#define STEER_PWM_MIN -100
#define STEER_PWM_MAX 100

#define constrain(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

//�ٶȻ�PID
IncPID pid_left = {11,0.5,0,0,  0,0,0}, pid_right = {11,0.5,0,0,0,0,0};

SteeringPID turnPID = {6.5, 0, 2.0};       // ת���⻷���������
SteeringPID inturnPID = {0.6, 0, 0.0001};     // ת���ڻ��������ǽ��ٶȿ���]

LowPassFilter pwmLeftFilter={0}; // ����PWM����˲���
LowPassFilter pwmRightFilter={0}; // ����PWM����˲���

//extern icm_param_t icm_datat;


int count = 0; 
int dir = 0;
char send_str[64];


/*
�ٶ�	kp		kd		kp		kd
200		8		1		0.5		0
250		9.8		2		0.6		0.0001		
275		6.5		2		0.6		0.0001	 
300		

*/








void Speed_Filter_init(void)
{
	// alphaֵ��Χ��0.0-1.0��ֵԽС�˲�Խǿ����ӦԽ��
	// �Ƽ�ֵ��
	// 0.1-0.2: ǿ�˲����ʺ�������Ļ���
	// 0.3-0.5: �е��˲���ƽ����Ӧ���ȶ���
	// 0.6-0.8: ���˲�����Ӧ�쵫���ܲ����ȶ�
	float pwm_filter_alpha = 0.8f;    // PWM����˲�����

	LowPassFilter_Init(&pwmLeftFilter, pwm_filter_alpha);       // ����PWM����˲���
	LowPassFilter_Init(&pwmRightFilter, pwm_filter_alpha);      // ����PWM����˲���
}

void Motor_Init(){
    gpio_init(DIR1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // GPIO ��ʼ��Ϊ��� Ĭ�����������
    gpio_init(DIR2, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // GPIO ��ʼ��Ϊ��� Ĭ�����������
    pwm_init(PWM1, 17000, 0);                                                   // PWM ͨ����ʼ��Ƶ�� 17KHz ռ�ձȳ�ʼΪ 0
    pwm_init(PWM2, 17000, 0);                                                   // PWM ͨ����ʼ��Ƶ�� 17KHz ռ�ձȳ�ʼΪ 0
	PID_Init();
	Speed_Filter_init();
    
}


void L_Motor_Speed(int Speed){
  
    if(Speed>4000) Speed = 4000;
    if(Speed<0) Speed = 0;
    if(Speed>=0){
        gpio_set_level(DIR1, GPIO_HIGH);                                    // DIR����ߵ�ƽ
        pwm_set_duty(PWM1, Speed);
    }

            
}



void R_Motor_Speed(int Speed){
    
    if(Speed>4000) Speed = 4000;
    if(Speed<0) Speed = 0;
    if(Speed>=0){
        gpio_set_level(DIR2, GPIO_HIGH);                                    // DIR����ߵ�ƽ
        pwm_set_duty(PWM2, Speed);
    }


}







void speed_control_L(int target_speed_L) {
   // ����Ŀ���ٶ�
    pid_left.target = target_speed_L;

    
    // ��ȡ������ʵ���ٶȣ���ȷ����λһ�£�
    int left_speed = Get_Encoder_L();

    
    // ����PID����
    int left_pwm = IncPID_Calculate(&pid_left, left_speed);

    
    // ����޷�������Ӳ��������
    left_pwm = constrain(left_pwm, -5000, 5000);

    //printf("L:%d \n",left_speed);
    // �������
    L_Motor_Speed(left_pwm);
	//printf("%d,%d\n",target_speed_L,left_speed);
	//ips200_show_int(30,140,left_pwm,3);
	//printf("left_speed:%d\n",left_pwm);
 
	
}

void speed_control_R(int target_speed_R) {
   // ����Ŀ���ٶ�
    pid_right.target = target_speed_R;

    
    // ��ȡ������ʵ���ٶȣ���ȷ����λһ�£�
    int right_speed = Get_Encoder_R();

    
    // ����PID����
    int right_pwm = IncPID_Calculate(&pid_right, right_speed);

    
    // ����޷�������Ӳ��������
    right_pwm = constrain(right_pwm, -5000, 5000);

    //printf("R:%d \n",right_speed);
    // �������
    R_Motor_Speed(right_pwm);
	
	//printf("%d,%d\n",target_speed_R,right_speed);
	
	//ips200_show_int(30,160,right_pwm,3);

}


float steering_control(float path_deviation) {
    // �⻷��·��ƫ�� -> Ŀ����ٶ�
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
    // �ڻ���������ʵ�ʽ��ٶȸ���
    float steer_pwm = steering_pd_control(&inturnPID, target_gyro, -icm_data.gyro_z);
	
    // ת������޷�
    return constrain(steer_pwm, STEER_PWM_MIN, STEER_PWM_MAX);
}




void combined_control(int base_speed, float path_deviation) {
    // 1. ת����ƣ���ȡת��ƫ����
    float steer_pwm = steering_control(path_deviation);
    
    // 2. �ǶԳƲ��ٲ���
    float speed_k = steer_pwm * 0.01f;
    int left_speed, right_speed;
	
	
    
    if(steer_pwm >= 0) { 
        // ��ת������С�����٣����ִ������
        left_speed = base_speed * (1 + speed_k * 0.1f);
        right_speed = base_speed * (1 - speed_k * 0.85f);
    } else { 
        // ��ת�����ִ�����٣�����С������
        left_speed = base_speed * (1 - (-speed_k) * 0.85f);
        right_speed = base_speed * (1 + (-speed_k) * 0.1f);
    }
    
	left_speed = (int16)LowPassFilter_Apply(&pwmLeftFilter, (float)left_speed);
	right_speed = (int16)LowPassFilter_Apply(&pwmRightFilter, (float)right_speed);
	
    // 3. �ٶȻ�����
	speed_control_L(left_speed);
	speed_control_R(right_speed);
	//ips200_show_int(0,0,(int)steer_pwm,3);
	

	
}

