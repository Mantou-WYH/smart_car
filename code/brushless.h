#ifndef _BRUSHLESS_H_
#define _BRUSHLESS_H_

#define PWM3                (TCPWM_CH53_P18_4)
#define PWM4                (TCPWM_CH52_P18_5)


void brushless_init();

void brushless_set_duty(int duty);



#endif