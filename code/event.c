#include "zf_common_headfile.h"


void start_runing(){					//����
	encoder_clear_count(ENCODER1);
	encoder_clear_count(ENCODER2);
	//BASE_SPEED = SPEED;
	pit_ms_init(PIT_CH2,5);			
	
}

void start_brushless(){					//������ѹ
	brushless_set_duty(30);
}

void brushless_stop(){					//�رո�ѹ
	brushless_set_duty(0);
}

void motor_stop(){						//ͣ��
	current_state = stop;
	current_circle_state = circle_stop;
}

void all_stop(){						//ͣ��ͣ��ѹ
	brushless_stop();
	motor_stop();
}