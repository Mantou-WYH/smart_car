#include "zf_common_headfile.h"
#include <stdint.h>
#include <string.h>

extern unsigned char Array_forward_bird_col[22560],Array_forward_bird_row[22560];


//二值化
//-------------------------------------------------------------------------------------------------------------------
//  @brief     二值化
//  @return     uint8
//  @since     
//  Sample usage:   threshold(img_input,img_output,MT9V03X_W,MT9V03X_H,100);
//---------------------------------------------------------------------------------------------------------------
void threshold(uint8_t *img_data, uint8_t *output_data, int width, int height, int thres){
  for(int y=0; y<height; y++){
	for(int x=0; x<width; x++){
	  output_data[x+y*width] = img_data[x+y*width]>thres ? 255 : 0;
	}
  }
}


/*begin  大津法比赛   begin*/
//快速大津法二值化 ;
//-------------------------------------------------------------------------------------------------------------------
//  @brief      快速大津
//  @return     uint8
//  @since      v1.1
//  Sample usage:   OTSU_Threshold = otsuThreshold(mt9v03x_image_dvp[0]);//大津法阈值
//-------------------------------------------------------------------------------------------------------------------
uint8 otsuThreshold_fast(uint8 *image)   //注意计算阈值的一定要是原图像
{
	#define GrayScale 256
	
    int Pixel_Max=0;
    int Pixel_Min=255;
    uint16 width = MT9V03X_W;   
    uint16 height = MT9V03X_H;  
    int pixelCount[GrayScale];  //各像素GrayScale的个数pixelCount 一维数组
    float pixelPro[GrayScale];  //各像素GrayScale所占百分比pixelPro 一维数组
    int i, j, pixelSum = width * height/4;  //pixelSum是获取总的图像像素个数的1/4，相应下面轮询时高和宽都是以2为单位自增
    int threshold = 0;
    uint8* data = image;  //指向像素数据的指针
 
    //清零
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
 
    uint32 gray_sum=0;  //每次执行到这会将gray_sum清零
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i+=2)   //高
    {
        for (j = 0; j < width; j+=2)    //宽
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
            if(data[i * width + j]>Pixel_Max)   Pixel_Max=data[i * width + j];
            if(data[i * width + j]<Pixel_Min)   Pixel_Min=data[i * width + j];
        }
    }
 
    //计算每个像素值的点在整幅图像中的比例
    for (i = Pixel_Min; i < Pixel_Max; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
 
    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;
 
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {
 
        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值
 
        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;
 
        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        deltaTmp = (float)(w0 *w1* (u0 - u1)* (u0 - u1)) ;
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = (uint8)j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
 
    }
	
	if(threshold<90){
        threshold = 90;
	}
	if(threshold>130)
        threshold = 130;

    return threshold;
}





// 腐蚀操作
void erosion(uint8_t *img_data, uint8_t *output_data, int width, int height, int kernel_size) {
    int half_kernel = kernel_size / 2;
    
    // 临时初始化为全白图像
    memset(output_data, 255, width * height * sizeof(uint8_t));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t min_val = 255;
            
            // 在结构元素范围内查找最小值
            for (int ky = -half_kernel; ky <= half_kernel; ky++) {
                for (int kx = -half_kernel; kx <= half_kernel; kx++) {
                    int nx = x + kx;
                    int ny = y + ky;
                    
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        if (img_data[nx + ny * width] < min_val) {
                            min_val = img_data[nx + ny * width];
                        }
                    }
                }
            }
            
            output_data[x + y * width] = min_val;
        }
    }
}

// 膨胀操作
void dilation(uint8_t *img_data, uint8_t *output_data, int width, int height, int kernel_size) {
    int half_kernel = kernel_size / 2;
    
    // 临时初始化为全黑图像
    memset(output_data, 0, width * height * sizeof(uint8_t));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t max_val = 0;
            
            // 在结构元素范围内查找最大值
            for (int ky = -half_kernel; ky <= half_kernel; ky++) {
                for (int kx = -half_kernel; kx <= half_kernel; kx++) {
                    int nx = x + kx;
                    int ny = y + ky;
                    
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        if (img_data[nx + ny * width] > max_val) {
                            max_val = img_data[nx + ny * width];
                        }
                    }
                }
            }
            
            output_data[x + y * width] = max_val;
        }
    }
}

// 先腐蚀后膨胀（开运算)
//-------------------------------------------------------------------------------------------------------------------
//  @brief     腐蚀膨胀
//  @return     uint8
//  @since      
//  Sample usage:   opening((uint8_t *)img_tmp,(uint8_t *)output_data,MT9V03X_W,MT9V03X_H,3);
//--------------------------------------------------------------------------------------------------------------
void opening(uint8_t *img_data, uint8_t *output_data, int width, int height, int kernel_size) {
    // 分配临时内存用于腐蚀结果
    uint8_t temp_data[MT9V03X_H][MT9V03X_W];
    
    // 先腐蚀
    erosion((uint8_t *)img_data, (uint8_t *)temp_data, width, height, kernel_size);
    
    // 再膨胀
    dilation((uint8_t *)temp_data, (uint8_t *)output_data, width, height, kernel_size);
    
    // 释放临时内存
}

//逆透视
void image_Perspective_Correction_Forward(uint8_t *img_data, uint8_t *output_data){
	for(int j=0; j<MT9V03X_H; j++){
		for (int i=0; i<MT9V03X_W;i++){
			int index = i+j*MT9V03X_W;
			output_data[index]=img_data[Array_forward_bird_row[index]*MT9V03X_W+Array_forward_bird_col[index]];
		}
	}
}

// 总运算，大津+二值
//-------------------------------------------------------------------------------------------------------------------
//  @brief     总值
//  @return     uint8
//  @since      
//  Sample usage:   opening((uint8_t *)img_tmp,(uint8_t *)output_data,MT9V03X_W,MT9V03X_H,3);
//---------------------------------------------------------------------------------------------------------
void img_main(uint8_t *img_data, uint8_t *output_data){
	//uint8_t img_tmp[MT9V03X_H][MT9V03X_W]; 

	int threshold_OUST = otsuThreshold_fast(img_data);

	threshold((uint8_t *)img_data,(uint8_t *)output_data,MT9V03X_W,MT9V03X_H,threshold_OUST);
	
	//image_Perspective_Correction_Forward((uint8_t *)img_tmp,(uint8_t *)output_data);

	//opening((uint8_t *)img_tmp,(uint8_t *)output_data,MT9V03X_W,MT9V03X_H,3);

	
}
