#ifndef _Encoder_H_
#define _Encoder_H_

#define PIT0                       		(PIT_CH0 )                             // 使用的周期中断编号

#define ENCODER1                     	(TC_CH09_ENCODER)                      // 编码器接口       
#define ENCODER1_QUADDEC_A             	(TC_CH09_ENCODER_CH1_P05_0)            // A引脚                      
#define ENCODER1_QUADDEC_B            	(TC_CH09_ENCODER_CH2_P05_1)            // B引脚                        
                                                                                
#define ENCODER2                     	(TC_CH07_ENCODER)                      // 编码器接口   
#define ENCODER2_QUADDEC_A            	(TC_CH07_ENCODER_CH1_P02_0)            // A引脚                  
#define ENCODER2_QUADDEC_B            	(TC_CH07_ENCODER_CH2_P02_1)            // B引脚          

void Encoder_Init();
int16 Get_Encoder_R();
int16 Get_Encoder_L();


#endif