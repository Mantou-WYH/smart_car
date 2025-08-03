#ifndef _Encoder_H_
#define _Encoder_H_

#define PIT0                       		(PIT_CH0 )                             // ʹ�õ������жϱ��

#define ENCODER1                     	(TC_CH09_ENCODER)                      // �������ӿ�       
#define ENCODER1_QUADDEC_A             	(TC_CH09_ENCODER_CH1_P05_0)            // A����                      
#define ENCODER1_QUADDEC_B            	(TC_CH09_ENCODER_CH2_P05_1)            // B����                        
                                                                                
#define ENCODER2                     	(TC_CH07_ENCODER)                      // �������ӿ�   
#define ENCODER2_QUADDEC_A            	(TC_CH07_ENCODER_CH1_P02_0)            // A����                  
#define ENCODER2_QUADDEC_B            	(TC_CH07_ENCODER_CH2_P02_1)            // B����          

void Encoder_Init();
int16 Get_Encoder_R();
int16 Get_Encoder_L();


#endif