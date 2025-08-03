#include "zf_common_headfile.h"

void Init_main(){
	gpio_init(LED1, GPO, GPIO_LOW, GPO_PUSH_PULL);    
	Motor_Init();
	Encoder_Init();
	imu660ra_init();
	gyroOffset_init();
	mt9v03x_init();
	Key_init();
	flash_init();
	ips200_init(IPS200_TYPE_SPI);
	mt9v03x_init();
	brushless_init();
	wireless_uart_init();
	
	pit_ms_init(PIT_CH1,1); 			//陀螺仪中断
	
	//timer_init(TC_TIME2_CH0, TIMER_US);
	timer_init(TC_TIME2_CH0, TIMER_MS);                                         // 圆环用计时器

}