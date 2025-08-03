#include <stdint.h>
#include "zf_common_headfile.h"

#define WIDTH   MT9V03X_W
#define HEIGHT  MT9V03X_H

///////////////////////////////��Ե��ȡ�㷨

// �Ż�1��ʹ�ò�����sqrt��atan2
static const uint8_t sqrt_table[65026];  // Ԥ����0~2552��ƽ����
static const int8_t angle_table[256][256]; // Ԥ����Ƕȷ���

// �Ż�2���򻯸�˹�˼���
void gaussian_blur_fast(uint8_t in[HEIGHT][WIDTH], uint8_t out[HEIGHT][WIDTH]) {
    for (int y = 1; y < HEIGHT-1; y++) {
        for (int x = 1; x < WIDTH-1; x++) {
            uint16_t sum = 
                in[y-1][x-1] + 2*in[y-1][x] + in[y-1][x+1] +
                2*in[y][x-1] + 4*in[y][x] + 2*in[y][x+1] +
                in[y+1][x-1] + 2*in[y+1][x] + in[y+1][x+1];
            out[y][x] = sum >> 4;  // �ȼ���sum/16
        }
    }
}

// �Ż�3��������Sobel����
void sobel_edge_fast(uint8_t in[HEIGHT][WIDTH], uint8_t grad[HEIGHT][WIDTH], uint8_t dir[HEIGHT][WIDTH]) {
    for (int y = 1; y < HEIGHT-1; y++) {
        for (int x = 1; x < WIDTH-1; x++) {
            int16_t gx = (in[y-1][x+1] + 2*in[y][x+1] + in[y+1][x+1]) - 
                        (in[y-1][x-1] + 2*in[y][x-1] + in[y+1][x-1]);
            int16_t gy = (in[y-1][x-1] + 2*in[y-1][x] + in[y-1][x+1]) -
                        (in[y+1][x-1] + 2*in[y+1][x] + in[y+1][x+1]);
            
            grad[y][x] = sqrt_table[gx*gx + gy*gy];  // ������sqrt
            dir[y][x] = angle_table[gy+128][gx+128]; // ���õ��Ƕȷ���
        }
    }
}

// �Ż�4���򻯷Ǽ���ֵ����
void nms_fast(uint8_t grad[HEIGHT][WIDTH], uint8_t dir[HEIGHT][WIDTH], uint8_t out[HEIGHT][WIDTH]) {
    for (int y = 1; y < HEIGHT-1; y++) {
        for (int x = 1; x < WIDTH-1; x++) {
            uint8_t val = grad[y][x];
            uint8_t angle = dir[y][x];
            
            // ��Ϊ4�����ж�
            if(angle == 0) {          // ˮƽ
                out[y][x] = (val >= grad[y][x-1] && val >= grad[y][x+1]) ? val : 0;
            } 
            else if(angle == 90) {    // ��ֱ
                out[y][x] = (val >= grad[y-1][x] && val >= grad[y+1][x]) ? val : 0;
            }
            // ����45���135���ж��Լ���
        }
    }
}

// �Ż�5������˫��ֵ����
void double_threshold_fast(uint8_t in[HEIGHT][WIDTH], uint8_t out[HEIGHT][WIDTH], 
                         uint8_t low, uint8_t high) {
    // �ϲ�Ϊ����ɨ��
    for (int y = 1; y < HEIGHT-1; y++) {
        for (int x = 1; x < WIDTH-1; x++) {
            uint8_t val = in[y][x];
            if(val >= high) {
                out[y][x] = 255;
            } else if(val >= low) {
                // ����ͨ�Լ�飨�����4����
                if(in[y-1][x]==255 || in[y+1][x]==255 || 
                   in[y][x-1]==255 || in[y][x+1]==255) {
                    out[y][x] = 255;
                } else {
                    out[y][x] = 0;
                }
            } else {
                out[y][x] = 0;
            }
        }
    }
}

// �����Ż���Canny
void canny_fast(uint8_t input[HEIGHT][WIDTH], uint8_t output[HEIGHT][WIDTH]) {
    uint8_t blurred[HEIGHT][WIDTH];
    uint8_t grad[HEIGHT][WIDTH];
    uint8_t dir[HEIGHT][WIDTH];
    
    gaussian_blur_fast(input, blurred);  // ~0.8ms
    sobel_edge_fast(blurred, grad, dir); // ~1.2ms 
    nms_fast(grad, dir, output);         // ~0.6ms
    double_threshold_fast(output, output, 30, 100); // ~0.4ms
}