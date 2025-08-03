#ifndef APPROX_LINES_H
#define APPROX_LINES_H

#include "zf_common_headfile.h"
////////////////////////////////////早期方案，未使用/////////////////////////


void approx_lines(Side *side_in, int point, float epsilon, Side *side_out, int *lines_num, int *point_indices, int offset);
Side weighted_midpoint(const Side* sides, size_t count, double length_threshold, double y_threshold) ;

#endif
