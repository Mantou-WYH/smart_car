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
        case 0:  // 第0页菜单
            ips200_show_string(40, 20, "running start");
            ips200_show_string(40, 40, "brushless start");	
            ips200_show_string(40, 60, "running stop");
            ips200_show_string(40, 60, "brushless stop");
            ips200_show_string(40, 60, "all stop");
            break;
            
        case 1:  // 第1页菜单
            ips200_show_string(40, 20, "");
            ips200_show_string(40, 40, "5. 程序5");
            ips200_show_string(40, 60, "6. 程序6");
            break;
            
        case 2:  // 第2页菜单
            ips200_show_string(40, 20, "7. 程序7");
            ips200_show_string(40, 40, "8. 程序8");
            ips200_show_string(40, 60, "9. 程序9");
            break;
            
        // 可以继续添加更多页面...
        
        default:  // 默认情况
            ips200_show_string(20, 20, "无效页面");
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
    int max_projects = 2;  // 默认最多3个项目（0-2）
    
    // 根据不同页面调整项目数量上限
    switch(mune_page) {
        case 0:
        case 1:
            max_projects = 2;  // 页面0和1有3个项目
            break;
        case 2:
            max_projects = 1;  // 页面2有2个项目
            break;
        // 其他页面的项目数量...
    }
    
    // 切换到下一个项目，到达上限后循环到第一个项目
    project_number = (project_number + 1) % (max_projects + 1);
}

void pre_project() {
    // 根据当前菜单页面确定项目数量上限
    int max_projects = 2;  // 默认最多3个项目（0-2）
    
    // 根据不同页面调整项目数量上限
    switch(mune_page) {
        case 0:
        case 1:
            max_projects = 2;  // 页面0和1有3个项目
            break;
        case 2:
            max_projects = 1;  // 页面2有2个项目
            break;
        // 其他页面的项目数量...
    }
    
    // 切换到上一个项目，到达下限后循环到最后一个项目
    project_number = (project_number - 1 + (max_projects + 1)) % (max_projects + 1);
}

void done(){
	// 根据菜单页面和项目编号执行对应的程序
    switch(mune_page) {
        case 0:  // 第0页菜单
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
            
        case 1:  // 第1页菜单
            switch(project_number) {
                case 0: 
                    program_4();
                    break;
                case 1: 
                    program_5();
                    break;
                
            }
            break;
            
        case 2:  // 第2页菜单
            switch(project_number) {
                case 0:  
                    program_6();
                    break;
                
            }
            break;
        
        default:  // 默认情况
            // 处理无效的菜单页面
            break;
    }
} 