#ifndef _MOTOR_H_
#define _MOTOR_H_


#define DIR2                (P00_2)
#define PWM2                (TCPWM_CH13_P00_3)
#define DIR1                (P18_6)
#define PWM1                (TCPWM_CH50_P18_7)

/*
#define TRACK_WIDTH 131.5f    // 轮距(mm)
#define WHEELBASE 42.5f      // 轴距(mm)
#define MIN_TURN_RADIUS 65.75f // 最小转弯半径(mm)
#define MAX_CURVATURE (1.0f / MIN_TURN_RADIUS)
#define MAX_MOTOR_SPEED 1000  // 电机最大转速值
*/


void Motor_Init();
void R_Motor_Speed(int Speed);
void L_Motor_Speed(int Speed);
void speed_control_L(int target_speed_L);
void speed_control_R(int target_speed_R);
void combined_control(int target_speed, float path_deviation);
float angle_control(float target_angle);
void start_runing();

#endif