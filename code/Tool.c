#include "zf_common_headfile.h"

extern SteeringPID turnPID,inturnPID;
float dt = 0.005f;


//-------------------------------------------------------------------------------------------------------------------
//  函数简介      PID控制器初始化
//  返回类型     void
//  使用样例     PID_Init();
//  说明：       清零所有PID控制器的历史误差，避免初次计算时的积分积累
//-------------------------------------------------------------------------------------------------------------------	
void PID_Init() {
    // 清零历史状态
    turnPID.error = turnPID.last_error = turnPID.last_last_error = 0;
    inturnPID.error = inturnPID.last_error = inturnPID.last_last_error = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      位置式PD控制器
//  返回类型     float（计算得到的控制量）
//	参数说明		pid PD控制器结构体指针
//              setpoint 目标值
//              actual 实际测量值
//  使用样例		float control = steering_pd_control(&turnPID, target_angle, actual_angle);
//  说明：       实现PD控制公式：output = Kp * e(t) + Kd * de(t)/dt
//              使用中心差分法计算微分项
//-------------------------------------------------------------------------------------------------------------------
float steering_pd_control(SteeringPID *pid, float setpoint, float actual) {
    float new_error = setpoint - actual;
	float derivative = (new_error - pid->last_error) / dt;  // 加入时间因子
	pid->last_last_error = pid->last_error;
	pid->last_error = new_error;
	pid->error = new_error;

	return (pid->Kp * new_error + pid->Kd * derivative);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      增量式PID控制器
//  返回类型     int（计算得到的控制量增量）
//	参数说明		pid PID控制器结构体指针
//              current_speed 当前测量值
//  使用样例		int speed_adjust = IncPID_Calculate(&motorPID, current_speed);
//  公式说明：   Δu(t) = Kp·[e(t)-e(t-1)] + Ki·e(t) + Kd·[e(t)-2e(t-1)+e(t-2)]
//-------------------------------------------------------------------------------------------------------------------
int IncPID_Calculate(IncPID *pid, int current_speed) {
    int error = pid->target - current_speed;
	
    
    // 计算PID各项
    int p_term = (int)pid->Kp * (error - pid->last_error);
    int i_term = (int)pid->Ki * error;
    int d_term = (int)pid->Kd * (error - 2 * pid->last_error + pid->prev_error);
    
    // 计算控制量增量
    int increment = p_term + i_term + d_term;
    
    // 计算本次输出
    int output = pid->last_output + increment;
    
    // 更新历史误差和输出
    pid->prev_error = pid->last_error;
    pid->last_error = error; 
    pid->last_output = output;
    
    return output;
}

//////////////调试用//////
//-------------------------------------------------------------------------------------------------------------------
//  函数简介      整数转字符串（安全实现）
//  返回类型     const char*（静态缓冲区地址）
//	参数说明		value 要转换的整数值
//  使用样例		const char* str = int_to_str(-12345);
//  注意：       返回静态缓冲区地址，每次调用会覆盖之前结果
//              支持最大10位数字（含符号）
//-------------------------------------------------------------------------------------------------------------------
const char* int_to_str(int value) {

    static char buffer[12]; 
    
    int len = snprintf(buffer, sizeof(buffer), "%d", value);
    
    if (len < 0 || len >= sizeof(buffer)) {
        return "ERR"; 
    }
    
    return buffer;
}
//-------------------------------------------------------------------------------------------------------------------
//  函数简介      无线串口发送两个整数值
//  返回类型     void
//	参数说明		a 第一个整数值
//              b 第二个整数值
//  使用样例		sent_two_int(current_error, pwm_output);
//  输出格式：    "a,b\n"
//-------------------------------------------------------------------------------------------------------------------
void sent_two_int(int a,int b){
	wireless_uart_send_string(int_to_str(a));
	wireless_uart_send_string(",");
	wireless_uart_send_string(int_to_str(b));
	wireless_uart_send_string("\n");
}

// -------------------- 滤波器相关 --------------------
// 初始化一阶低通滤波器
void LowPassFilter_Init(LowPassFilter *filter, float alpha)
{
    filter->alpha = alpha;
    filter->previous_output = 0.0f;
}

// 应用一阶低通滤波器
float LowPassFilter_Apply(LowPassFilter *filter, float sample)
{
    filter->previous_output = filter->alpha * sample + (1.0f - filter->alpha) * filter->previous_output;
    return filter->previous_output;
}

	
	