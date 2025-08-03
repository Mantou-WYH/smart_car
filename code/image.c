#include "zf_common_headfile.h"
#include <stdint.h>
#include <string.h>

extern unsigned char Array_forward_bird_col[22560],Array_forward_bird_row[22560];


//��ֵ��
//-------------------------------------------------------------------------------------------------------------------
//  @brief     ��ֵ��
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


/*begin  ��򷨱���   begin*/
//���ٴ�򷨶�ֵ�� ;
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ���ٴ��
//  @return     uint8
//  @since      v1.1
//  Sample usage:   OTSU_Threshold = otsuThreshold(mt9v03x_image_dvp[0]);//�����ֵ
//-------------------------------------------------------------------------------------------------------------------
uint8 otsuThreshold_fast(uint8 *image)   //ע�������ֵ��һ��Ҫ��ԭͼ��
{
	#define GrayScale 256
	
    int Pixel_Max=0;
    int Pixel_Min=255;
    uint16 width = MT9V03X_W;   
    uint16 height = MT9V03X_H;  
    int pixelCount[GrayScale];  //������GrayScale�ĸ���pixelCount һά����
    float pixelPro[GrayScale];  //������GrayScale��ռ�ٷֱ�pixelPro һά����
    int i, j, pixelSum = width * height/4;  //pixelSum�ǻ�ȡ�ܵ�ͼ�����ظ�����1/4����Ӧ������ѯʱ�ߺͿ�����2Ϊ��λ����
    int threshold = 0;
    uint8* data = image;  //ָ���������ݵ�ָ��
 
    //����
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
 
    uint32 gray_sum=0;  //ÿ��ִ�е���Ὣgray_sum����
    //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    for (i = 0; i < height; i+=2)   //��
    {
        for (j = 0; j < width; j+=2)    //��
        {
            pixelCount[(int)data[i * width + j]]++;  //����ǰ�ĵ������ֵ��Ϊ����������±�
            gray_sum+=(int)data[i * width + j];       //�Ҷ�ֵ�ܺ�
            if(data[i * width + j]>Pixel_Max)   Pixel_Max=data[i * width + j];
            if(data[i * width + j]<Pixel_Min)   Pixel_Min=data[i * width + j];
        }
    }
 
    //����ÿ������ֵ�ĵ�������ͼ���еı���
    for (i = Pixel_Min; i < Pixel_Max; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
 
    //�����Ҷȼ�[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;
 
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {
 
        w0 += pixelPro[j];  //��������ÿ���Ҷ�ֵ�����ص���ռ����֮��   ���������ֵı���
        u0tmp += j * pixelPro[j];  //�������� ÿ���Ҷ�ֵ�ĵ�ı��� *�Ҷ�ֵ
 
        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;
 
        u0 = u0tmp / w0;              //����ƽ���Ҷ�
        u1 = u1tmp / w1;              //ǰ��ƽ���Ҷ�
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





// ��ʴ����
void erosion(uint8_t *img_data, uint8_t *output_data, int width, int height, int kernel_size) {
    int half_kernel = kernel_size / 2;
    
    // ��ʱ��ʼ��Ϊȫ��ͼ��
    memset(output_data, 255, width * height * sizeof(uint8_t));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t min_val = 255;
            
            // �ڽṹԪ�ط�Χ�ڲ�����Сֵ
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

// ���Ͳ���
void dilation(uint8_t *img_data, uint8_t *output_data, int width, int height, int kernel_size) {
    int half_kernel = kernel_size / 2;
    
    // ��ʱ��ʼ��Ϊȫ��ͼ��
    memset(output_data, 0, width * height * sizeof(uint8_t));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t max_val = 0;
            
            // �ڽṹԪ�ط�Χ�ڲ������ֵ
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

// �ȸ�ʴ�����ͣ�������)
//-------------------------------------------------------------------------------------------------------------------
//  @brief     ��ʴ����
//  @return     uint8
//  @since      
//  Sample usage:   opening((uint8_t *)img_tmp,(uint8_t *)output_data,MT9V03X_W,MT9V03X_H,3);
//--------------------------------------------------------------------------------------------------------------
void opening(uint8_t *img_data, uint8_t *output_data, int width, int height, int kernel_size) {
    // ������ʱ�ڴ����ڸ�ʴ���
    uint8_t temp_data[MT9V03X_H][MT9V03X_W];
    
    // �ȸ�ʴ
    erosion((uint8_t *)img_data, (uint8_t *)temp_data, width, height, kernel_size);
    
    // ������
    dilation((uint8_t *)temp_data, (uint8_t *)output_data, width, height, kernel_size);
    
    // �ͷ���ʱ�ڴ�
}

//��͸��
void image_Perspective_Correction_Forward(uint8_t *img_data, uint8_t *output_data){
	for(int j=0; j<MT9V03X_H; j++){
		for (int i=0; i<MT9V03X_W;i++){
			int index = i+j*MT9V03X_W;
			output_data[index]=img_data[Array_forward_bird_row[index]*MT9V03X_W+Array_forward_bird_col[index]];
		}
	}
}

// �����㣬���+��ֵ
//-------------------------------------------------------------------------------------------------------------------
//  @brief     ��ֵ
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
