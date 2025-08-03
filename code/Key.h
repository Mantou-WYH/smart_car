#ifndef _KEY_H
#define _KEY_H

extern uint8_t Keyflag[5];
extern uint8_t start;

#define KEY1                    (P11_0)
#define KEY2                    (P22_0)
#define KEY3                    (P23_3)
#define KEY4                    (P23_4)

typedef struct {
    uint8_t state;       // 0:�ȴ����� 1:������ 2:�ȴ��ɿ�
    uint8_t flag;        // ���±�־�������أ�
    uint16_t debounce;   // ����������
} Key_t;


void Key_init();
uint8_t readKey(gpio_pin_enum key);
void key_event();
void Key_update();
void Key_reset(uint8_t i);

#endif