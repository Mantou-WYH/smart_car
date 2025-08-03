#ifndef _MAZE_H_
#define _MAZE_H_
 
#define max(a,b) a>b?a:b
#define min(a,b) a>b?b:a

#define SAFE_MARGIN_Y 20

typedef struct {
    uint16_t x;
    uint16_t y;
} Side;

typedef struct {
    int start_offset;
    int end_offset;
    int step;
    int y_start;
    int y_end;
} SearchPattern;


void Line_search_main(uint8_t img_data[MT9V03X_H][MT9V03X_W]);
uint8_t safe_access_img(uint8_t img[MT9V03X_H][MT9V03X_W], int x, int y);
int get_R_point();
int get_L_point();
void show_point(Side point);

#endif