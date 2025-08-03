#ifndef _TOOL_H_
#define _TOOL_H_

extern float dt;

typedef struct {
	float Kp, Ki, Kd;
    int target;         // 目标速度
    int last_error;     // 上次误差
    int prev_error;     // 上上次误差
    int last_output;    // 上次输出
} IncPID;

typedef struct {
    float Kp, Ki, Kd;
    float error, last_error, last_last_error;
} SteeringPID;


typedef struct {
    float alpha; // 滤波器系数
    float previous_output; // 上一次的输出值
} LowPassFilter;

// ================= 函数声明 =================
// 位置式PD控制
float steering_pd_control(SteeringPID *pid, float setpoint, float actual);

// 增量式PID控制
int IncPID_Calculate(IncPID *pid, int current_speed);

// 整数转字符串
const char* int_to_str(int value);

// 无线发送两个整数
void sent_two_int(int a, int b);

// 初始化低通滤波器
void LowPassFilter_Init(LowPassFilter *filter, float alpha);

// 应用低通滤波
float LowPassFilter_Apply(LowPassFilter *filter, float sample);


void PID_Init();

#endif