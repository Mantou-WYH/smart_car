#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "zf_common_headfile.h"

#define POINTS 2048  // 自定义上限，够用即可

////////////////////////////////////早期方案，未使用/////////////////////////

extern const uint8 weight[120];


void approx_lines(Side *side_in, int point, float epsilon, Side *side_out, int *lines_num, int *point_indices, int offset) {
    if (point < 2) {
        *lines_num = 0;
        return;
    }

    int dx = side_in[point - 1].x - side_in[0].x;
    int dy = side_in[point - 1].y - side_in[0].y;
    float length = sqrtf(dx * dx + dy * dy);
    if (length == 0) {
        *lines_num = 0;
        return;
    }
    float nx = -dy / length;
    float ny = dx / length;

    float max_dist = 0;
    float dist;
    int idx = -1;

    for (int i = 1; i < point - 1; i++) {
        dist = fabs((side_in[i].x - side_in[0].x) * nx + (side_in[i].y - side_in[0].y) * ny);
        if (dist > max_dist) {
            max_dist = dist;
            idx = i;
        }
    }

    if (max_dist >= epsilon) {
        int num1 = 0, num2 = 0;
        approx_lines(side_in, idx + 1, epsilon, side_out, &num1, point_indices, offset);
        approx_lines(side_in + idx, point - idx, epsilon, side_out + num1, &num2, point_indices + num1, offset + idx);
        *lines_num = num1 + num2 - 1;
    } else {
        side_out[0] = side_in[0];
        side_out[1] = side_in[point - 1];
        point_indices[0] = offset;
        point_indices[1] = offset + point - 1;
        *lines_num = 2;
    }
}


Side weighted_midpoint(const Side* sides, size_t count, double length_threshold, double y_threshold) {
    double total_weight = 0.0;
    double weighted_sum_x = 0.0;
    double weighted_sum_y = 0.0;

    for (size_t i = 0; i + 1 < count; i++) {
        double x1 = sides[i].x;
        double y1 = sides[i].y;
        double x2 = sides[i + 1].x;
        double y2 = sides[i + 1].y;

        double dx = x2 - x1;
        double dy = y2 - y1;
        double length = sqrt(dx * dx + dy * dy);

		double mid_x = (x1 + x2) / 2.0;
		double mid_y = (y1 + y2) / 2.0;


		weighted_sum_x += mid_x * weight[MT9V03X_H-1-(int)mid_y];
		weighted_sum_y += mid_y * weight[MT9V03X_H-1-(int)mid_y];
		total_weight += weight[MT9V03X_H-1-(int)mid_y];
	

        
    }

    Side result;
    if (total_weight == 0.0) {
        result.x = 0;
        result.y = 0;
    } else {
        result.x = (uint16_t)(weighted_sum_x / total_weight + 0.5); // 四舍五入
        result.y = (uint16_t)(weighted_sum_y / total_weight + 0.5);
    }

    return result;
}



