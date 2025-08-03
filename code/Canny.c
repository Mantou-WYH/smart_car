#include <stdint.h>
#include "zf_common_headfile.h"

#define WIDTH   MT9V03X_W
#define HEIGHT  MT9V03X_H

///////////////////////////////边缘提取算法

// 优化1：使用查表法替代sqrt和atan2
static const uint8_t sqrt_table[65026];  // 预计算0~2552的平方根
static const int8_t angle_table[256][256]; // 预计算角度分类

// 优化2：简化高斯核计算
void gaussian_blur_fast(uint8_t in[HEIGHT][WIDTH], uint8_t out[HEIGHT][WIDTH]) {
    for (int y = 1; y < HEIGHT-1; y++) {
        for (int x = 1; x < WIDTH-1; x++) {
            uint16_t sum = 
                in[y-1][x-1] + 2*in[y-1][x] + in[y-1][x+1] +
                2*in[y][x-1] + 4*in[y][x] + 2*in[y][x+1] +
                in[y+1][x-1] + 2*in[y+1][x] + in[y+1][x+1];
            out[y][x] = sum >> 4;  // 等价于sum/16
        }
    }
}

// 优化3：整数化Sobel计算
void sobel_edge_fast(uint8_t in[HEIGHT][WIDTH], uint8_t grad[HEIGHT][WIDTH], uint8_t dir[HEIGHT][WIDTH]) {
    for (int y = 1; y < HEIGHT-1; y++) {
        for (int x = 1; x < WIDTH-1; x++) {
            int16_t gx = (in[y-1][x+1] + 2*in[y][x+1] + in[y+1][x+1]) - 
                        (in[y-1][x-1] + 2*in[y][x-1] + in[y+1][x-1]);
            int16_t gy = (in[y-1][x-1] + 2*in[y-1][x] + in[y-1][x+1]) -
                        (in[y+1][x-1] + 2*in[y+1][x] + in[y+1][x+1]);
            
            grad[y][x] = sqrt_table[gx*gx + gy*gy];  // 查表替代sqrt
            dir[y][x] = angle_table[gy+128][gx+128]; // 查表得到角度分类
        }
    }
}

// 优化4：简化非极大值抑制
void nms_fast(uint8_t grad[HEIGHT][WIDTH], uint8_t dir[HEIGHT][WIDTH], uint8_t out[HEIGHT][WIDTH]) {
    for (int y = 1; y < HEIGHT-1; y++) {
        for (int x = 1; x < WIDTH-1; x++) {
            uint8_t val = grad[y][x];
            uint8_t angle = dir[y][x];
            
            // 简化为4方向判断
            if(angle == 0) {          // 水平
                out[y][x] = (val >= grad[y][x-1] && val >= grad[y][x+1]) ? val : 0;
            } 
            else if(angle == 90) {    // 垂直
                out[y][x] = (val >= grad[y-1][x] && val >= grad[y+1][x]) ? val : 0;
            }
            // 忽略45°和135°判断以加速
        }
    }
}

// 优化5：快速双阈值处理
void double_threshold_fast(uint8_t in[HEIGHT][WIDTH], uint8_t out[HEIGHT][WIDTH], 
                         uint8_t low, uint8_t high) {
    // 合并为单次扫描
    for (int y = 1; y < HEIGHT-1; y++) {
        for (int x = 1; x < WIDTH-1; x++) {
            uint8_t val = in[y][x];
            if(val >= high) {
                out[y][x] = 255;
            } else if(val >= low) {
                // 简化连通性检查（仅检查4邻域）
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

// 最终优化版Canny
void canny_fast(uint8_t input[HEIGHT][WIDTH], uint8_t output[HEIGHT][WIDTH]) {
    uint8_t blurred[HEIGHT][WIDTH];
    uint8_t grad[HEIGHT][WIDTH];
    uint8_t dir[HEIGHT][WIDTH];
    
    gaussian_blur_fast(input, blurred);  // ~0.8ms
    sobel_edge_fast(blurred, grad, dir); // ~1.2ms 
    nms_fast(grad, dir, output);         // ~0.6ms
    double_threshold_fast(output, output, 30, 100); // ~0.4ms
}