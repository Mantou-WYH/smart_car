#include "zf_common_headfile.h"

static Key_t keys[4];    // 4 个按键状态
static const gpio_pin_enum key_pin[4] = {KEY1, KEY2, KEY3, KEY4};

uint8_t start = 0;

/////////////////////写完才发现重复造轮子了，逐飞库里就有按键的函数/////////////////////////////

void Key_init(){
	gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);
}

uint8_t readKey(gpio_pin_enum key){
	return gpio_get_level(key);
}

void Key_update() {
    for (int i = 0; i < 4; i++) {
        uint8_t level = readKey(key_pin[i]);

        switch (keys[i].state) {
            case 0: // 等待按下
                if (level == 0) {
                    keys[i].debounce = 0;
                    keys[i].state = 1;
                }
                break;

            case 1: // 消抖中
                if (level == 0) {
					keys[i].flag = 1;    // 设置按下事件
					keys[i].state = 2;   // 等待松开
                } else {
                    keys[i].state = 0; // 抖动
                }
                break;

            case 2: // 等待松开
                if (level == 1) {
                    keys[i].state = 0;
                }
                break;
        }
    }
}


void Key_reset(uint8_t i){
	keys[i].flag = 0;
}

void key_event() {
    for (int i = 0; i < 4; i++) {
        if (keys[i].flag) {
            keys[i].flag = 0; // 重置事件

		switch (i) {
			case 0:
				Navigation_point();
				break;
			case 1:
				system_delay_ms(1000);
				start = 1;
				start_runing();
				// 处理按键2
				break;
			case 2:
				start_brushless();
				// 处理按键3
				break;
			case 3:
				brushless_stop();
				//ips200_show_int(0,40,1,1);
				// 处理按键4
				break;
            }
        }
    }
}
