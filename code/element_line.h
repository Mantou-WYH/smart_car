#ifndef ELEMENT_LINE_H
#define ELEMENT_LINE_H

extern int element_line_L,element_line_R,element_line_U,element_line_B,element_line_M;
extern Side L_edge[10],R_edge[10],U_edge[10],B_edge[10],M_edge[10];
extern int circle_l,circle_r;

void element_update(uint8_t img[MT9V03X_H][MT9V03X_W]);
uint8_t circle_L(uint8_t img[MT9V03X_H][MT9V03X_W]);
uint8_t circle_R(uint8_t img[MT9V03X_H][MT9V03X_W]);




#endif