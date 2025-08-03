#include "zf_common_headfile.h"

extern SteeringPID turnPID,inturnPID;
float dt = 0.005f;


//-------------------------------------------------------------------------------------------------------------------
//  �������      PID��������ʼ��
//  ��������     void
//  ʹ������     PID_Init();
//  ˵����       ��������PID����������ʷ��������μ���ʱ�Ļ��ֻ���
//-------------------------------------------------------------------------------------------------------------------	
void PID_Init() {
    // ������ʷ״̬
    turnPID.error = turnPID.last_error = turnPID.last_last_error = 0;
    inturnPID.error = inturnPID.last_error = inturnPID.last_last_error = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      λ��ʽPD������
//  ��������     float������õ��Ŀ�������
//	����˵��		pid PD�������ṹ��ָ��
//              setpoint Ŀ��ֵ
//              actual ʵ�ʲ���ֵ
//  ʹ������		float control = steering_pd_control(&turnPID, target_angle, actual_angle);
//  ˵����       ʵ��PD���ƹ�ʽ��output = Kp * e(t) + Kd * de(t)/dt
//              ʹ�����Ĳ�ַ�����΢����
//-------------------------------------------------------------------------------------------------------------------
float steering_pd_control(SteeringPID *pid, float setpoint, float actual) {
    float new_error = setpoint - actual;
	float derivative = (new_error - pid->last_error) / dt;  // ����ʱ������
	pid->last_last_error = pid->last_error;
	pid->last_error = new_error;
	pid->error = new_error;

	return (pid->Kp * new_error + pid->Kd * derivative);
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ����ʽPID������
//  ��������     int������õ��Ŀ�����������
//	����˵��		pid PID�������ṹ��ָ��
//              current_speed ��ǰ����ֵ
//  ʹ������		int speed_adjust = IncPID_Calculate(&motorPID, current_speed);
//  ��ʽ˵����   ��u(t) = Kp��[e(t)-e(t-1)] + Ki��e(t) + Kd��[e(t)-2e(t-1)+e(t-2)]
//-------------------------------------------------------------------------------------------------------------------
int IncPID_Calculate(IncPID *pid, int current_speed) {
    int error = pid->target - current_speed;
	
    
    // ����PID����
    int p_term = (int)pid->Kp * (error - pid->last_error);
    int i_term = (int)pid->Ki * error;
    int d_term = (int)pid->Kd * (error - 2 * pid->last_error + pid->prev_error);
    
    // �������������
    int increment = p_term + i_term + d_term;
    
    // ���㱾�����
    int output = pid->last_output + increment;
    
    // ������ʷ�������
    pid->prev_error = pid->last_error;
    pid->last_error = error; 
    pid->last_output = output;
    
    return output;
}

//////////////������//////
//-------------------------------------------------------------------------------------------------------------------
//  �������      ����ת�ַ�������ȫʵ�֣�
//  ��������     const char*����̬��������ַ��
//	����˵��		value Ҫת��������ֵ
//  ʹ������		const char* str = int_to_str(-12345);
//  ע�⣺       ���ؾ�̬��������ַ��ÿ�ε��ûḲ��֮ǰ���
//              ֧�����10λ���֣������ţ�
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
//  �������      ���ߴ��ڷ�����������ֵ
//  ��������     void
//	����˵��		a ��һ������ֵ
//              b �ڶ�������ֵ
//  ʹ������		sent_two_int(current_error, pwm_output);
//  �����ʽ��    "a,b\n"
//-------------------------------------------------------------------------------------------------------------------
void sent_two_int(int a,int b){
	wireless_uart_send_string(int_to_str(a));
	wireless_uart_send_string(",");
	wireless_uart_send_string(int_to_str(b));
	wireless_uart_send_string("\n");
}

// -------------------- �˲������ --------------------
// ��ʼ��һ�׵�ͨ�˲���
void LowPassFilter_Init(LowPassFilter *filter, float alpha)
{
    filter->alpha = alpha;
    filter->previous_output = 0.0f;
}

// Ӧ��һ�׵�ͨ�˲���
float LowPassFilter_Apply(LowPassFilter *filter, float sample)
{
    filter->previous_output = filter->alpha * sample + (1.0f - filter->alpha) * filter->previous_output;
    return filter->previous_output;
}

	
	