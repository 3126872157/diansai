#ifndef __PID_H__
#define __PID_H__

#include "struct_typedef.h"

enum PID_MODE
{
    PID_POSITION = 0,
    PID_DELTA
};

typedef struct
{
    uint8_t mode;
    //PID参数
    fp32 Kp;
    fp32 Ki;
    fp32 Kd;

    fp32 max_out;  //最大输出
    fp32 max_iout; //积分限幅

    fp32 set;
    fp32 fdb;

    fp32 out;
    fp32 Pout;
    fp32 Iout;
    fp32 Dout;
    fp32 Dbuf[3];  //d项增益；0为这次，1为上次，2为上上次
    fp32 error[3]; //误差；   同上

} pid_type_def;

extern void PID_init(pid_type_def *pid, uint8_t mode, const fp32 PID[3], fp32 max_out, fp32 max_iout);
extern fp32 PID_calc(pid_type_def *pid, fp32 ref, fp32 set);
extern void PID_clear(pid_type_def *pid);

void my_pid_init(pid_type_def *pid1, pid_type_def *pid2,pid_type_def *pid3, fp32 max_out1, fp32 max_iout1, fp32 max_out2, fp32 max_iout2, fp32 max_out3, fp32 max_iout3);

#endif
