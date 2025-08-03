#ifndef _TOOL_H_
#define _TOOL_H_

extern float dt;

typedef struct {
	float Kp, Ki, Kd;
    int target;         // Ŀ���ٶ�
    int last_error;     // �ϴ����
    int prev_error;     // ���ϴ����
    int last_output;    // �ϴ����
} IncPID;

typedef struct {
    float Kp, Ki, Kd;
    float error, last_error, last_last_error;
} SteeringPID;


typedef struct {
    float alpha; // �˲���ϵ��
    float previous_output; // ��һ�ε����ֵ
} LowPassFilter;

// ================= �������� =================
// λ��ʽPD����
float steering_pd_control(SteeringPID *pid, float setpoint, float actual);

// ����ʽPID����
int IncPID_Calculate(IncPID *pid, int current_speed);

// ����ת�ַ���
const char* int_to_str(int value);

// ���߷�����������
void sent_two_int(int a, int b);

// ��ʼ����ͨ�˲���
void LowPassFilter_Init(LowPassFilter *filter, float alpha);

// Ӧ�õ�ͨ�˲�
float LowPassFilter_Apply(LowPassFilter *filter, float sample);


void PID_Init();

#endif