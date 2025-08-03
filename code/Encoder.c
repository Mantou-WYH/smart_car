#include "zf_common_headfile.h"


#define PIT0                       		(PIT_CH0 )                             // 使用的周期中断编号

#define ENCODER1                     	(TC_CH09_ENCODER)                      // 编码器接口       
#define ENCODER1_QUADDEC_A             	(TC_CH09_ENCODER_CH1_P05_0)            // A引脚                      
#define ENCODER1_QUADDEC_B            	(TC_CH09_ENCODER_CH2_P05_1)            // B引脚                        
                                                                                
#define ENCODER2                     	(TC_CH07_ENCODER)                      // 编码器接口   
#define ENCODER2_QUADDEC_A            	(TC_CH07_ENCODER_CH1_P02_0)            // A引脚                  
#define ENCODER2_QUADDEC_B            	(TC_CH07_ENCODER_CH2_P02_1)            // B引脚      

#define FILTER_ALPHA 0.15f  // 滤波系数 (0.1-0.3之间)

void Encoder_Init(){
  
    encoder_dir_init(ENCODER1, ENCODER1_QUADDEC_A, ENCODER1_QUADDEC_B);       // 初始化编码器模块与引脚 带方向增量编码器模式
    encoder_dir_init(ENCODER2, ENCODER2_QUADDEC_A, ENCODER2_QUADDEC_B);       // 初始化编码器模块与引脚 带方向增量编码器模式

  
}


int16_t Get_Encoder_L() {
    static float filtered_right = 0;  // 静态变量保存滤波后的值
    
    // 获取原始值并清零计数器
    int16_t raw = encoder_get_count(ENCODER1);
    encoder_clear_count(ENCODER1);
    
    // 应用低通滤波公式
    filtered_right = FILTER_ALPHA * raw + (1 - FILTER_ALPHA) * filtered_right;
    
    // 返回整数值（四舍五入）
    return (int16_t)(filtered_right + 0.5f);
}

int16_t Get_Encoder_R() {
    static float filtered_left = 0;  // 静态变量保存滤波后的值
    
    // 获取原始值并清零计数器
    int16_t raw = encoder_get_count(ENCODER2);
    encoder_clear_count(ENCODER2);
    
    // 应用低通滤波公式
    filtered_left = FILTER_ALPHA * raw + (1 - FILTER_ALPHA) * filtered_left;
    
    // 返回整数值（四舍五入）
    return -(int16_t)(filtered_left + 0.5f);
}