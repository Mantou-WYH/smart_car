#include "zf_common_headfile.h"
#include "math.h"



Side Side_L[200],Side_R[200];
Side Smooth_R[50],Smooth_L[50];
Side StarM;

Side Max_Lx,Max_Ly,Max_Rx,Max_Ry;



int touch,jump_dir = 1;
int L_turn[50],R_turn[50];
int	dashed;


extern Side Side_M[200];
extern car_state current_state;
extern car_circle_state current_circle_state;
extern int circle_dir;

int L_point,R_point;
int broken_l,broken_r;



const int dir_front[4][2]= {{0,  -1},
							{1,  0},
							{0,  1},
							{-1, 0}};

const int dir_frontleft[4][2] = {{-1, -1},
								 {1,  -1},
								 {1,  1}, 
								 {-1, 1}};

const int dir_frontright[4][2] ={{1,  -1},
								 {1,  1},
								 {-1, 1},
								 {-1, -1}};


//-------------------------------------------------------------------------------------------------------------------
// 	�������     ��ȫͼ����ʣ�Խ������һ�ɷ���0����ɫ��
// 	��������     uint8��255����0��
//	����˵��	img ͼ������
//				x��y Ҫ���ʵ���������
//	ʹ������	safe_access_img(img,0,0);
//---------------------------------------------------------------------------------------------------------------
inline uint8_t safe_access_img(uint8_t img[MT9V03X_H][MT9V03X_W], int x, int y) {
    if (x >= 0 && x < MT9V03X_W && y >= 0 && y < MT9V03X_H) {
        return img[y][x];
    } else {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  �������		��ʮ����ʾ���λ�ã������ã�
//  ��������     
//	����˵��		point Ҫ��ʾ�ĵ�
//  ʹ������		show_point(Side_L[0]);
//---------------------------------------------------------------------------------------------------------------
void show_point(Side point){
	if(point.x>10&&point.y>10&&point.x<MT9V03X_W&&point.y<MT9V03X_H){
	ips200_draw_line(point.x-10,point.y,point.x+10,point.y,RGB565_RED);
	ips200_draw_line(point.x,point.y-10,point.x,point.y+10,RGB565_RED);
	}
}


////////findline_twoside�ýṹ��////////
typedef struct {
	int fx;
	int fy;
	int fsx;
	int fsy;
	int step;
	int dir;
	int turn;
}maze;

//-------------------------------------------------------------------------------------------------------------------
//  �������     �����Թ���������߽��������е�����
//  ��������		�����أ�ֱ�Ӵ洢��ȫ�ֱ���Side_L��Side_R������   
//	����˵��		img ͼ������
//				Lx��Ly��Rx��Ry���������ʼ���xy�������ұ�����ʼ���xy����
//				*Lnum��*Rnum	�Ѿ������ĵ���
//				MAX_step			������߸���
//  Sample usage:   findline_twoside(img_data,Side_L[0].x,Side_L[0].y,Side_R[0].x,Side_R[0].y,&point_L,&point_R,Max_step);
//---------------------------------------------------------------------------------------------------------------
void findline_twoside(uint8_t img[MT9V03X_H][MT9V03X_W],int Lx,int Ly,int Rx,int Ry,int *Lnum,int *Rnum,int MAX_step){
	Max_Ly.y = MT9V03X_H-1;
	Max_Ry.y = MT9V03X_H-1;
	maze L={0},R={0};
	L.step = 0;
	R.step = 0;
	int L_stop = 0;
	int R_stop = 0;
	int start_x = (Lx+Rx)/2;
	if(safe_access_img(img,Lx,Ly) == 0) {
        L.step = 0;
		L_stop = 1;
    }
	if(safe_access_img(img,Rx,Ry) == 0) {
        R.step = 0;
		R_stop = 1;
    }
	if(L_stop==1&&R_stop==1) {
        return;
    }
	
	while(L.step+*Lnum<MAX_step && R.step+*Rnum<MAX_step && Ly<=MT9V03X_H-1 && Ry<=MT9V03X_H-1 && Ry>SAFE_MARGIN_Y && Ly>SAFE_MARGIN_Y && Lx>0 && Rx>0 && Lx<MT9V03X_W-1 && Rx<MT9V03X_W-1 && R.turn<4  && L.turn<4){
		//�����Թ���
		int front_Lx = Lx + dir_front[L.dir][0];
		int front_Ly = Ly + dir_front[L.dir][1];
        int frontleft_x = Lx + dir_frontleft[L.dir][0];
        int frontleft_y = Ly + dir_frontleft[L.dir][1];
		
        int front_L_value = safe_access_img(img, front_Lx, front_Ly);
        int frontleft_value = safe_access_img(img, frontleft_x, frontleft_y);

		if (front_L_value == 0) {
			L.dir = (L.dir + 1) % 4;
			L.turn++;
		} else if (frontleft_value == 0) {
			Lx += dir_front[L.dir][0];
			Ly += dir_front[L.dir][1];
			Side_L[L.step+*Lnum].x = Lx;
			Side_L[L.step+*Lnum].y = Ly;
			L.step++;
			L.turn = 0;
		} else {
			Lx += dir_frontleft[L.dir][0];
			Ly += dir_frontleft[L.dir][1];
			L.dir = (L.dir + 3) % 4;
			L.step++;
			Side_L[L.step+*Lnum].x = Lx;
			Side_L[L.step+*Lnum].y = Ly;
			L.turn = 0;
		}
		if(Max_Lx.x>Lx){
			Max_Lx.x = Lx;
			Max_Lx.y = Ly;
		}
		if(Max_Ly.y>Ly){
			Max_Ly.y = Ly;
			Max_Ly.x = Lx;
		}
		
		//�����Թ���
		
		int front_x = Rx + dir_front[R.dir][0];
        int front_y = Ry + dir_front[R.dir][1];
        int frontright_x = Rx + dir_frontright[R.dir][0];
        int frontright_y = Ry + dir_frontright[R.dir][1];

        int front_value = safe_access_img(img, front_x, front_y);
        int frontright_value = safe_access_img(img, frontright_x, frontright_y);

		if (front_value == 0) {
			R.dir = (R.dir + 3) % 4;
			R.turn++;
		} else if (frontright_value == 0) {
			Rx += dir_front[R.dir][0];
			Ry += dir_front[R.dir][1];
			Side_R[R.step+*Rnum].x = Rx;
			Side_R[R.step+*Rnum].y = Ry;
			R.step++;
			R.turn = 0;
		} else {
			Rx += dir_frontright[R.dir][0];
			Ry += dir_frontright[R.dir][1];
			R.dir = (R.dir + 1) % 4;
			R.step++;
			Side_R[R.step+*Rnum].x = Rx;
			Side_R[R.step+*Rnum].y = Ry;
			R.turn = 0;
		}
		if(Max_Rx.x<Rx){
			Max_Rx.x = Rx;
			Max_Rx.y = Ry;
		}
		if(Max_Ry.y>Ry){
			Max_Ry.y = Ry;
			Max_Ry.x = Rx;
		}
		if(R.step>5&&L.step>5){
			int dx = abs(Rx - Lx);
			int dy = abs(Ry - Ly);
			
			if((dx*dx+dy*dy)<=4){
				touch = 1;
				break;
			}
		}
	}
	
	if(touch == 1){
		if(start_x<(Lx+Rx)/2){
			jump_dir = 1;
		}else{
			jump_dir = 2;
		}
		
	}
	
	*Lnum += L.step;
	*Rnum += R.step;
	
}



//-------------------------------------------------------------------------------------------------------------------
//  �������		ͨ�������ȷ��ĳ�е���ʼ��
//  ��������     
//	����˵��		n	Ѱ���ĵ㱣����Side_L��Side_R�ĵڼ�λ
//				y	����һ��Ѱ��
//  ʹ������		First_Line_M(img_data,0,MT9V03X_H-1);
//-------------------------------------------------------------------------------------------------------------
uint8_t First_Line_M(uint8_t img_data[MT9V03X_H][MT9V03X_W],int n,int y){ //�����Ϊ��ʼ��
	int sum = 0;
	int sum_max = 0;
	StarM.x = MT9V03X_W / 2;  // Ĭ��ȡͼ������x����
    StarM.y = y;   // Ĭ��ȡ��ײ�y����
	
	for(int i=0;i<MT9V03X_W-1;i++){
		for(int j=y;j>0;j--){	
			if(safe_access_img(img_data, i, j) == 0) break;
			sum++;	
			
		}
		if(sum>sum_max){
			sum_max = sum;
			StarM.x = i;
			StarM.y = y;
			
		}
		sum = 0;
	}
	Side_L[n].x = StarM.x;
	Side_L[n].y = StarM.y;
	Side_R[n].x = StarM.x;
	Side_R[n].y = StarM.y;
	//ips200_show_int(0,120,sum_max,3);
	if(sum_max<1) return 0;
	

	
	
	
	for(uint16_t i=StarM.x;i>2;i--){
		if(safe_access_img(img_data,i-1,StarM.y)==0){
			if(safe_access_img(img_data,i-2,StarM.y)==0){
				Side_L[n].x = i;
				break;
			}
		}
	}
	
	for(uint16_t i=StarM.x;i<MT9V03X_W-2;i++){
		if(safe_access_img(img_data,i+1,StarM.y)==0){
			if(safe_access_img(img_data,i+2,StarM.y)==0){
				Side_R[n].x = i;
				break;
			}
		}
	}
	return 1;
	
}


//-------------------------------------------------------------------------------------------------------------------
//  �������		��������Ѱ�Ұ׵㣬����������
//  ��������     0��û�ҵ�����1���ҵ��ˣ�
//	����˵��		x	��ʼ��xֵ
//				start_y��end_y	��ʼ��yֵ��������yֵ
//				*search	�����ҵ��İ׵�����
//  ʹ������		search_line_up(img_data, x, p.y_start, p.y_end, &search)��
//-------------------------------------------------------------------------------------------------------------
uint8_t search_line_up(uint8_t img_data[MT9V03X_H][MT9V03X_W],int x,int start_y,int end_y,Side* search){
	if (x < 0 || x >= MT9V03X_W || start_y < 0 || end_y >= MT9V03X_H || start_y < end_y) {
        return 0; 
    }
	for (int y = start_y; y >= end_y; y--) {
        if (safe_access_img(img_data, x, y) == 255) { 
			if(safe_access_img(img_data,x,y-1)==255){
				if(safe_access_img(img_data,x,y-2)==255){
            // �ҵ���ɫ���أ���¼λ�ò����سɹ�
					search->x = x;
					search->y = y;
					return 1;
				}
			}
		}
		if(y<SAFE_MARGIN_Y){
			return 0;
		}
		
    }
    return 0; // δ�ҵ���ɫ����
}

 
//-------------------------------------------------------------------------------------------------------------------
//  �������      ����ʼ������/�������߽�㣨�ڵ㵽�׵�����䣩
//  ��������     void���޷���ֵ��
//	����˵��		img_data ͼ������
//				start ��ʼ�����꣨�õ�Ӧ��Ϊ��ɫ�㣩
//				n �� m �ֱ������ұ߽��Ĵ洢λ�ã�Side_L���������n��Side_R���������m��
//  ʹ������		search_boundary(img_data, StarM, 0, 0);
//---------------------------------------------------------------------------------------------------------
void search_boundary(uint8_t img_data[MT9V03X_H][MT9V03X_W],Side start,int n,int m){
	for(uint16_t i=start.x;i>2;i--){
		if(safe_access_img(img_data,i-1,start.y)==0){
				Side_L[n].x = i;
				Side_L[n].y = start.y;
				break;
			
		}
	}
	
	for(uint16_t i=start.x;i<MT9V03X_W-2;i++){
		if(safe_access_img(img_data,i+1,start.y)==0){
				Side_R[m].x = i;
				Side_R[m].y = start.y;
				break;
			
		}
	}
	
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ����ʼ���ˮƽ�����������ߵ㣬�ж�Ϊ�����߶κ�ʹ�ã�û����
//  ��������     uint8_t��0��ʧ�ܣ�1���ɹ���
//	����˵��		img_data ͼ������
//				start ��ʼ������
//				n/m ���ұ߽��Ĵ洢����
//  ʹ������		search_horizontal(img_data, jump_point, point_L, point_R);
//---------------------------------------------------------------------------------------------------------------
uint8_t search_horizontal(uint8_t img_data[MT9V03X_H][MT9V03X_W], Side start, int n,int m) {
	switch (jump_dir){
		case 1:{
			Side_R[m] = start;
			for(uint16_t i=start.y;i>1;i--){
				if(safe_access_img(img_data,start.x,i-1)==0){
						Side_L[n].x = start.x;
						Side_L[n].y = i;
						return 1;
						break;
					
				}
			}
			break;
		}
		case 2:{
			Side_L[n] = start;
			for(uint16_t i=start.y;i>1;i--){
				if(safe_access_img(img_data,start.x,i-1)==0){
						Side_R[m].x = start.x;
						Side_R[m].y = i;
						return 1;
						break;
					
				}
			}
			break;
		}
		
		
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      �������������ܣ�����ʼ�㸽����������Ŀ���
//  ��������     uint8_t��0��δ�ҵ����ߵ㣬1���ҵ����ߵ㣩
//	����˵��		img_data ͼ������
//				start ��ʼ�����꣨ͨ�����ѷ��ֵı߽���ߵ㣩
//				*result �洢�ҵ������ߵ�����
//  ʹ������		if(search_block(img_data, start_point, &jump_point)) {...}
//---------------------------------------------------------------------------------------------------------------
uint8_t search_block(uint8_t img_data[MT9V03X_H][MT9V03X_W], Side start, Side* result) {	//��������
    Side search;
    
    // ===== ����1�����峣������ =====
    const int UP_HIGH = 30;        // ���������߶�
    const int DOWN_HIGH = 25;       // ���������߶�
    const int UP_DOWN_WIDE = 70;    // ����������
    const int STEP_SIZE = 2;        // ��������
    
    // ===== ����2�����㶯̬������Χ =====
    int UP_WIDE_L = Max_Lx.x - start.x - 2;
    int UP_WIDE_R = Max_Rx.x - start.x + 2;
    int in_out_flag = (abs(UP_WIDE_L - UP_WIDE_R) < 40) ? 1 : 2;
    
    // �¹��ܣ��������ĵ�ƫ�ƣ�������in_out_flag == 1��
    int center_offset = 0;
    if (in_out_flag == 1) {
        // �������ұ߽���м��
        int center_x = (Max_Lx.x + Max_Rx.x) / 2;
        center_offset = center_x - start.x;
    }
    
    // ===== ����3����������ģʽ =====
    SearchPattern patterns[4] = {0}; // ��ʼ������Ԫ��Ϊ0
    int pattern_count = 0;           // ʵ��ʹ��ģʽ����
    
    // ����jump_dirѡ����������
    switch (jump_dir) {
        case 1: // ���Һ�������
            if (in_out_flag == 1) {
                // �����ĵ㿪ʼ���� 
                // ����
                patterns[0] = (SearchPattern){center_offset,UP_WIDE_R, STEP_SIZE,start.y,start.y - UP_HIGH};
                // ����
                patterns[1] = (SearchPattern){center_offset,UP_WIDE_L,-STEP_SIZE,start.y,start.y - UP_HIGH};
                // ������չ
                patterns[2] = (SearchPattern){UP_WIDE_R,UP_DOWN_WIDE,STEP_SIZE,start.y + DOWN_HIGH,start.y - UP_HIGH};
                // ������չ
                patterns[3] = (SearchPattern){UP_WIDE_L,-UP_DOWN_WIDE,-STEP_SIZE,start.y + DOWN_HIGH,start.y - UP_HIGH};
                pattern_count = 4;
            } else { 
                patterns[0] = (SearchPattern){0, UP_WIDE_R, STEP_SIZE, start.y, start.y - UP_HIGH};
                patterns[1] = (SearchPattern){UP_WIDE_R, UP_DOWN_WIDE, STEP_SIZE, start.y + DOWN_HIGH, start.y - UP_HIGH};
                patterns[2] = (SearchPattern){0, UP_WIDE_L, -STEP_SIZE, start.y, start.y - UP_HIGH};
                patterns[3] = (SearchPattern){UP_WIDE_L, -UP_DOWN_WIDE, -STEP_SIZE, start.y + DOWN_HIGH, start.y - UP_HIGH};
                pattern_count = 4;
            }
            break;
            
        case 2: // �����������
            if (in_out_flag == 1) {
                // �����ĵ���������
                patterns[0] = (SearchPattern){ center_offset,UP_WIDE_L,-STEP_SIZE,start.y,start.y - UP_HIGH};
                // �����ĵ���������
                patterns[1] = (SearchPattern){center_offset,UP_WIDE_R,STEP_SIZE,start.y,start.y - UP_HIGH};
                // ������չ����
                patterns[2] = (SearchPattern){UP_WIDE_L,-UP_DOWN_WIDE,-STEP_SIZE,start.y + DOWN_HIGH,start.y - UP_HIGH};
                // ������չ����
                patterns[3] = (SearchPattern){UP_WIDE_R,UP_DOWN_WIDE,STEP_SIZE,start.y + DOWN_HIGH,start.y - UP_HIGH};
                pattern_count = 4;
            } else { 
                patterns[0] = (SearchPattern){0, UP_WIDE_L, -STEP_SIZE, start.y, start.y - UP_HIGH};
                patterns[1] = (SearchPattern){UP_WIDE_L, -UP_DOWN_WIDE, -STEP_SIZE, start.y + DOWN_HIGH, start.y - UP_HIGH};
                patterns[2] = (SearchPattern){0, UP_WIDE_R, STEP_SIZE, start.y, start.y - UP_HIGH};
                patterns[3] = (SearchPattern){UP_WIDE_R, UP_DOWN_WIDE, STEP_SIZE, start.y + DOWN_HIGH, start.y - UP_HIGH};
                pattern_count = 4;
            }
            break;
            
        default: 
            return 0;
    }
    
    // ===== ����4��ִ������ģʽ =====
    for (int i = 0; i < pattern_count; i++) {
        SearchPattern p = patterns[i];
        
        int sign = (p.step > 0) ? 1 : -1;
        if (p.step == 0) continue; 
        
 
        int start_offset = p.start_offset;
        int end_offset = p.end_offset;
        
        // ִ�е���������
        for (int offset = start_offset; 
             sign * offset <= sign * end_offset; 
             offset += p.step) 
        {
            int x = start.x + offset;
            
            // ͼ��߽籣��
            if (x < 0 || x >= MT9V03X_W) {
                break; // ����ͼ��߽磬ֹͣ�����˷���
            }
            
            if (search_line_up(img_data, x, p.y_start, p.y_end, &search)) {
                *result = search; // �ҵ��߽磬���ؽ��
                return 1;        // �ɹ��ҵ�
            }
        }
    }
    
    // ===== ����5��δ�ҵ�����ʧ�� =====
    return 0;
}


//-------------------------------------------------------------------------------------------------------------------
//  �������      �ж���ʼ�㴦�Ƿ���ں��ߣ�������߳������Դ��ڴ�ֱ��������Ϊ�Ǻ��ߣ�
//  ��������     uint8_t��0�����Ǻ��ߣ�1���Ǻ��ߣ�
//	����˵��		img_data ͼ������
//				start ��ʼ������
//  ʹ������		if(detect_horizontal_line(img_data, jump_point)) {...}
//  ע�⣺�˺�����һЩ����»����У����δʹ��
//---------------------------------------------------------------------------------------------------------------
uint8_t detect_horizontal_line(uint8_t img_data[MT9V03X_H][MT9V03X_W], Side start) {			
    int horizontal_count_L = 0;
	int horizontal_count_R = 0;
    int vertical_count = 0;
	
	// ͳ�ƴ�ֱ��ɫ��������
	while(safe_access_img(img_data,start.x,start.y+vertical_count)==255){
		vertical_count++;
	}
    
    // ͳ�ƺ����ɫ��������
    while(safe_access_img(img_data,start.x+horizontal_count_R,start.y+vertical_count/2)==255){
		horizontal_count_R++;
	}
	
    while(safe_access_img(img_data,start.x-horizontal_count_L,start.y+vertical_count/2)==255){
		horizontal_count_L++;
	}
    
	if(horizontal_count_R>horizontal_count_L){
		jump_dir = 1;
	}else{
		jump_dir = 2;
	}
    // ��������������Զ��ڴ�ֱ���أ��ж�Ϊ�����߶�
	ips200_show_int(0,140,vertical_count,3);
	ips200_show_int(0,160,horizontal_count_L+horizontal_count_R,3);
	return vertical_count*2<(horizontal_count_L+horizontal_count_R);
}


void Line_search_main(uint8_t img_data[MT9V03X_H][MT9V03X_W]){
	
	
	/////////////////////��ʼ��/////////////////////////////
	touch = 0;
	jump_dir = 0;
	dashed = 0;
	int point_L = 0,point_R = 0;
	int Max_step = 200;
	int check = 0;
	int jump_check = 0;
	Side jump_point;
	Max_Lx.x = MT9V03X_W-1;
	Max_Rx.x = 0;
	Max_Ly.y = MT9V03X_H-1;
	Max_Ry.y = MT9V03X_H-1;
	/////////////////////��ʼ��/////////////////////////////	
	
	///////////��·����/////////////
	if(current_state == open_run){																		
		for(int i=70;i>SAFE_MARGIN_Y+30;i=i-10){
			if(check==0){
				check = First_Line_M(img_data,0,i);
			}else{
				break;
			}
		}
		if(check==1){
			findline_twoside(img_data,Side_L[0].x,Side_L[0].y,Side_R[0].x,Side_R[0].y,&point_L,&point_R,Max_step);
			//ips200_show_string(0,120,"!!!!!");
			noOpen();
		}
	}
	/////////////ʮ����Ծ����///////////////////
	else if(current_state == cross && L_edge[0].y>30 && R_edge[0].y>30){								
		int cross_y = min(L_edge[0].y,R_edge[0].y);
		for(int i=cross_y-5;i>SAFE_MARGIN_Y;i=i-5){
			if(check==0){
				check = First_Line_M(img_data,0,i);
			}else{
				break;
			}
		}
				
		findline_twoside(img_data,Side_L[0].x,Side_L[0].y,Side_R[0].x,Side_R[0].y,&point_L,&point_R,Max_step);
	}
	/////////////////��������///////////////////
	else{																								
		
		check = First_Line_M(img_data,0,MT9V03X_H-1);
		
		if(check==0){
			for(int i=MT9V03X_H-2;i>MT9V03X_H-32;i=i-3){
				if(check==0){
					check = First_Line_M(img_data,0,i);
				}else{
					break;
				}
			}
			for(int i=MT9V03X_H-32;i>SAFE_MARGIN_Y-20;i=i-10){
				if(check==0){
					check = First_Line_M(img_data,0,i);
				}else{
					break;
				}
			}
		}
		
		findline_twoside(img_data,Side_L[0].x,Side_L[0].y,Side_R[0].x,Side_R[0].y,&point_L,&point_R,Max_step);

		
	}
	///////////////��Զ���߲�����////////////////
	if(touch==1&&point_L>=150&&point_R>=150){
		touch = 0;
	}
	
	//////////////////��������///////////////////
	while(touch==1){
		
		touch = 0;
		Side start_point = Max_Ly.y<Max_Ry.y?Max_Ly:Max_Ry;

		jump_check = search_block(img_data,start_point,&jump_point);
		if(jump_check== 1){/*;
			if(detect_horizontal_line(img_data,jump_point)){
				if(circle_dir==1){
					jump_dir = 2;
				}else if(circle_dir==2){
					jump_dir = 1;
				}
				search_horizontal(img_data,jump_point,point_L,point_R);
			}else{
				search_boundary(img_data,jump_point,point_L,point_R);	
			}
			*/
			
			findline_twoside(img_data,jump_point.x,jump_point.y,jump_point.x,jump_point.y,&point_L,&point_R,Max_step);
		}else if(point_L<15&&point_R<15){
			isOpen();
			//gpio_set_level(LED1,1);
		}
	}

	L_point = point_L;
	R_point = point_R;

}



int get_L_point(){
	return L_point;
}

int get_R_point(){
	return R_point;
}

