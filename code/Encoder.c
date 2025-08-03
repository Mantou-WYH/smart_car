#include "zf_common_headfile.h"


#define PIT0                       		(PIT_CH0 )                             // ʹ�õ������жϱ��

#define ENCODER1                     	(TC_CH09_ENCODER)                      // �������ӿ�       
#define ENCODER1_QUADDEC_A             	(TC_CH09_ENCODER_CH1_P05_0)            // A����                      
#define ENCODER1_QUADDEC_B            	(TC_CH09_ENCODER_CH2_P05_1)            // B����                        
                                                                                
#define ENCODER2                     	(TC_CH07_ENCODER)                      // �������ӿ�   
#define ENCODER2_QUADDEC_A            	(TC_CH07_ENCODER_CH1_P02_0)            // A����                  
#define ENCODER2_QUADDEC_B            	(TC_CH07_ENCODER_CH2_P02_1)            // B����      

#define FILTER_ALPHA 0.15f  // �˲�ϵ�� (0.1-0.3֮��)

void Encoder_Init(){
  
    encoder_dir_init(ENCODER1, ENCODER1_QUADDEC_A, ENCODER1_QUADDEC_B);       // ��ʼ��������ģ�������� ����������������ģʽ
    encoder_dir_init(ENCODER2, ENCODER2_QUADDEC_A, ENCODER2_QUADDEC_B);       // ��ʼ��������ģ�������� ����������������ģʽ

  
}


int16_t Get_Encoder_L() {
    static float filtered_right = 0;  // ��̬���������˲����ֵ
    
    // ��ȡԭʼֵ�����������
    int16_t raw = encoder_get_count(ENCODER1);
    encoder_clear_count(ENCODER1);
    
    // Ӧ�õ�ͨ�˲���ʽ
    filtered_right = FILTER_ALPHA * raw + (1 - FILTER_ALPHA) * filtered_right;
    
    // ��������ֵ���������룩
    return (int16_t)(filtered_right + 0.5f);
}

int16_t Get_Encoder_R() {
    static float filtered_left = 0;  // ��̬���������˲����ֵ
    
    // ��ȡԭʼֵ�����������
    int16_t raw = encoder_get_count(ENCODER2);
    encoder_clear_count(ENCODER2);
    
    // Ӧ�õ�ͨ�˲���ʽ
    filtered_left = FILTER_ALPHA * raw + (1 - FILTER_ALPHA) * filtered_left;
    
    // ��������ֵ���������룩
    return -(int16_t)(filtered_left + 0.5f);
}