#include "zf_common_headfile.h"

int mune_page = 0;
int	project_number = 0;

void mune_init{
	mune_page = 0;
	project_number = 0;
	
}

void mune_show(){
	ips200_show_string(0,(project_number+1)*20,"--");
	ips200_show_int(200,0,mune_page)
	switch(mune_page) {
        case 0:  // ��0ҳ�˵�
            ips200_show_string(40, 20, "running start");
            ips200_show_string(40, 40, "brushless start");	
            ips200_show_string(40, 60, "running stop");
            ips200_show_string(40, 60, "brushless stop");
            ips200_show_string(40, 60, "all stop");
            break;
            
        case 1:  // ��1ҳ�˵�
            ips200_show_string(40, 20, "");
            ips200_show_string(40, 40, "5. ����5");
            ips200_show_string(40, 60, "6. ����6");
            break;
            
        case 2:  // ��2ҳ�˵�
            ips200_show_string(40, 20, "7. ����7");
            ips200_show_string(40, 40, "8. ����8");
            ips200_show_string(40, 60, "9. ����9");
            break;
            
        // ���Լ�����Ӹ���ҳ��...
        
        default:  // Ĭ�����
            ips200_show_string(20, 20, "��Чҳ��");
            break;
	
}


void next_page{
	mune_page++;
	project_number = 0;
}

void pre_page{
	if(mune_page>0){
		mune_page--;
		project_number = 0;
	}
}

void next_project() {
    int max_projects = 2;  // Ĭ�����3����Ŀ��0-2��
    
    // ���ݲ�ͬҳ�������Ŀ��������
    switch(mune_page) {
        case 0:
        case 1:
            max_projects = 2;  // ҳ��0��1��3����Ŀ
            break;
        case 2:
            max_projects = 1;  // ҳ��2��2����Ŀ
            break;
        // ����ҳ�����Ŀ����...
    }
    
    // �л�����һ����Ŀ���������޺�ѭ������һ����Ŀ
    project_number = (project_number + 1) % (max_projects + 1);
}

void pre_project() {
    // ���ݵ�ǰ�˵�ҳ��ȷ����Ŀ��������
    int max_projects = 2;  // Ĭ�����3����Ŀ��0-2��
    
    // ���ݲ�ͬҳ�������Ŀ��������
    switch(mune_page) {
        case 0:
        case 1:
            max_projects = 2;  // ҳ��0��1��3����Ŀ
            break;
        case 2:
            max_projects = 1;  // ҳ��2��2����Ŀ
            break;
        // ����ҳ�����Ŀ����...
    }
    
    // �л�����һ����Ŀ���������޺�ѭ�������һ����Ŀ
    project_number = (project_number - 1 + (max_projects + 1)) % (max_projects + 1);
}

void done(){
	// ���ݲ˵�ҳ�����Ŀ���ִ�ж�Ӧ�ĳ���
    switch(mune_page) {
        case 0:  // ��0ҳ�˵�
            switch(project_number) {
                case 0:  
                    program_1();
                    break;
                case 1: 
                    program_2();
                    break;
                case 2:  
                    program_3();
                    break;
                
            }
            break;
            
        case 1:  // ��1ҳ�˵�
            switch(project_number) {
                case 0: 
                    program_4();
                    break;
                case 1: 
                    program_5();
                    break;
                
            }
            break;
            
        case 2:  // ��2ҳ�˵�
            switch(project_number) {
                case 0:  
                    program_6();
                    break;
                
            }
            break;
        
        default:  // Ĭ�����
            // ������Ч�Ĳ˵�ҳ��
            break;
    }
} 