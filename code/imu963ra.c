#include "zf_common_headfile.h"
#include <math.h>

#define delta_T     0.001f  //10ms计算一次


float I_ex, I_ey, I_ez;  // 误差积分

quater_param_t Q_info = {1, 0, 0};  // 全局四元数
euler_param_t eulerAngle; //欧拉角

icm_param_t icm_data;
gyro_param_t GyroOffset;

int16 GyroOffset_init = 0;

float param_Kp = 0.1;   // 加速度计的收敛速率比例增益
float param_Ki = 0.001;   //陀螺仪收敛速率的积分增益 0.004


float fast_sqrt(float x)
{
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long *) &y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *) &i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     陀螺仪零飘初始化
// 使用示例     gyroOffset_init();
// 备注信息     如果初始化后依旧漂的很厉害，可以把下面循环次数中100增加（确保小车是静止时执行此函数）
// 作    者     16届上交大开源代码
// 修改时间     2024 - 01
//-------------------------------------------------------------------------------------------------------------------
void gyroOffset_init(void)      /////////陀螺仪零飘初始化
{
	
    GyroOffset.Xdata = 0;
    GyroOffset.Ydata = 0;
    GyroOffset.Zdata = 0;
	
    Q_info.q0 = 1;
    Q_info.q1 = 0;
    Q_info.q2 = 0;
    Q_info.q3 = 0;
	
    for (uint16_t i = 0; i < 500; ++i)
    {
       imu963ra_get_gyro();
       imu963ra_get_acc();

       GyroOffset.Xdata += imu660ra_gyro_x;
       GyroOffset.Ydata += imu660ra_gyro_y;
       GyroOffset.Zdata += imu660ra_gyro_z;
       system_delay_ms(5);
    }

    GyroOffset.Xdata /= 500;
    GyroOffset.Ydata /= 500;
    GyroOffset.Zdata /= 500;

    GyroOffset_init = 1;
	

}


#define alpha           0.3f

//转化为实际物理值
void ICM_getValues(void)
{
    //一阶低通滤波，单 位g/s
    icm_data.acc_x = (((float) imu660ra_acc_x) * alpha) * 8 / 2048 + icm_data.acc_x * (1 - alpha);
    icm_data.acc_y = (((float) imu660ra_acc_y) * alpha) * 8 / 2048 + icm_data.acc_y * (1 - alpha);
    icm_data.acc_z = (((float) imu660ra_acc_z) * alpha) * 8 / 2048 + icm_data.acc_z * (1 - alpha);


    //陀螺仪角度转弧度
    icm_data.gyro_x = ((float) imu660ra_gyro_x - GyroOffset.Xdata) * M_PI / 180 / 16.4f;
    icm_data.gyro_y = ((float) imu660ra_gyro_y - GyroOffset.Ydata) * M_PI / 180 / 16.4f;
    icm_data.gyro_z = ((float) imu660ra_gyro_z - GyroOffset.Zdata) * M_PI / 180 / 16.4f;
}


//互补滤波
void ICM_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az)
{
    float halfT = 0.5 * delta_T;
    float vx, vy, vz;    //当前的机体坐标系上的重力单位向量
    float ex, ey, ez;    //四元数计算值与加速度计测量值的误差
    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;
    float q0q0 = q0 * q0;
    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
 //   float q0q3 = q0 * q3;
    float q1q1 = q1 * q1;
 //   float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q3q3 = q3 * q3;
    // float delta_2 = 0;

    //对加速度数据进行归一化 得到单位加速度
    float norm = fast_sqrt(ax * ax + ay * ay + az * az);
    ax = ax * norm;
    ay = ay * norm;
    az = az * norm;

    //根据当前四元数的姿态值来估算出各重力分量。用于和加速计实际测量出来的各重力分量进行对比，从而实现对四轴姿态的修正
    vx = 2 * (q1q3 - q0q2);
    vy = 2 * (q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3;
    //vz = (q0*q0-0.5f+q3 * q3) * 2;

    //叉积来计算估算的重力和实际测量的重力这两个重力向量之间的误差。
    ex = ay * vz - az * vy;
    ey = az * vx - ax * vz;
    ez = ax * vy - ay * vx;

    //用叉乘误差来做PI修正陀螺零偏，
    //通过调节 param_Kp，param_Ki 两个参数，
    //可以控制加速度计修正陀螺仪积分姿态的速度。
    I_ex += halfT * ex;   // integral error scaled by Ki
    I_ey += halfT * ey;
    I_ez += halfT * ez;

    gx = gx + param_Kp * ex + param_Ki * I_ex;
    gy = gy + param_Kp * ey + param_Ki * I_ey;
    gz = gz + param_Kp * ez + param_Ki * I_ez;


    /*数据修正完成，下面是四元数微分方程*/


    //四元数微分方程，其中halfT为测量周期的1/2，gx gy gz为陀螺仪角速度，以下都是已知量，这里使用了一阶龙哥库塔求解四元数微分方程
    q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
    q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
    q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;
    //    delta_2=(2*halfT*gx)*(2*halfT*gx)+(2*halfT*gy)*(2*halfT*gy)+(2*halfT*gz)*(2*halfT*gz);
    // 整合四元数率    四元数微分方程  四元数更新算法，二阶毕卡法
    //    q0 = (1-delta_2/8)*q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
    //    q1 = (1-delta_2/8)*q1 + (q0*gx + q2*gz - q3*gy)*halfT;
    //    q2 = (1-delta_2/8)*q2 + (q0*gy - q1*gz + q3*gx)*halfT;
    //    q3 = (1-delta_2/8)*q3 + (q0*gz + q1*gy - q2*gx)*halfT


    // normalise quaternion
    norm = fast_sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    Q_info.q0 = q0 * norm;
    Q_info.q1 = q1 * norm;
    Q_info.q2 = q2 * norm;
    Q_info.q3 = q3 * norm;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     陀螺仪零飘初始化
// 使用示例     ICM_getEulerianAngles();
// 备注信息
// 作    者     16届上交大开源代码
// 修改时间     2024 - 01
//-------------------------------------------------------------------------------------------------------------------
/*把四元数转换成欧拉角*/
void ICM_getEulerianAngles(void)
{
    //采集陀螺仪数据
    imu660ra_get_gyro();
    imu660ra_get_acc();		
    
    static float last_yaw = 0;  // 保存上一次的 yaw 值
    float delta_yaw = 0;        // yaw 的变化量

    ICM_getValues();
    ICM_AHRSupdate(icm_data.gyro_x, icm_data.gyro_y, icm_data.gyro_z, icm_data.acc_x, icm_data.acc_y, icm_data.acc_z);
    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;

    //四元数计算欧拉角
    eulerAngle.pitch = (float)asin(-2 * q1 * q3 + 2 * q0 * q2) * 180 / M_PI; // pitch
    eulerAngle.roll = (float)atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 180 / M_PI; // roll
    eulerAngle.yaw = (float)atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * 180 / M_PI ; // yaw
    
    delta_yaw = eulerAngle.yaw - last_yaw;
    
     if (fabs(delta_yaw) < 0.1f) {
        eulerAngle.yaw = last_yaw;  // 恢复上一次的 yaw 值
    } else {
        last_yaw = eulerAngle.yaw;  // 更新上一次的 yaw 值
    }

    


/*   姿态限制*/
    if (eulerAngle.roll > 90 || eulerAngle.roll < -90) {
        if (eulerAngle.pitch > 0) {
            eulerAngle.pitch = 180 - eulerAngle.pitch;
        }
        if (eulerAngle.pitch < 0) {
            eulerAngle.pitch = -(180 + eulerAngle.pitch);
        }
    }

    if (eulerAngle.yaw >= 360) {
        eulerAngle.yaw -= 360;
    } else if (eulerAngle.yaw < 0) {
        eulerAngle.yaw += 360;
    }
    
    if(eulerAngle.yaw >180){
        eulerAngle.yaw -= 360;
    }
    
}


void show_angle(){
	ips200_show_int(0,120,(int)get_yaw(),3);
	//ips200_show_int(0,20,(int)get_pitch(),3);
	//ips200_show_int(0,40,(int)get_roll(),3);
	
}

float get_pitch(){

  
    return eulerAngle.pitch;
  
}

float get_yaw(){
  
    return eulerAngle.yaw;
  
}

float get_roll(){
  
    return eulerAngle.roll;

}
