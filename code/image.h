#ifndef _IMAGES_H_
#define _IMAGES_H_
 

 
void threshold(uint8_t *img_data, uint8_t *output_data, int width, int height, int thres);
uint8 otsuThreshold_fast(uint8 *image);
void img_main(uint8_t *img_data, uint8_t *output_data);


#endif

/*

typedef struct {
    int x;
    int y;
} Side;

Side Side_L[MT9V03X_H],Side_R[MT9V03X_H],Side_M[MT9V03X_H]; 

*/