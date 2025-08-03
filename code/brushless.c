#include "zf_common_headfile.h"

//////////////////////PWM������ˢ��ѹ////////////////////////
// ��ϧû���ü�����û����

void brushless_init(){
	pwm_init(PWM3, 50, 0);
    pwm_init(PWM4, 50, 0);
}


void brushless_set_duty(int duty){
	if(duty>=0&&duty<=100){
		duty = duty*10 + 500;
		pwm_set_duty(PWM3,duty);
		pwm_set_duty(PWM4,duty);	
	}else{
		pwm_set_duty(PWM3,500);
		pwm_set_duty(PWM4,500);	
	}
}



