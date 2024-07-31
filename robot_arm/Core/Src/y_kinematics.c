#include "y_kinematics.h"
#include <math.h>

#define pi 3.1415926
#define cos60 0.5f
#define cos45 0.707f

kinematics_t kinematics;

//找最小角度用test
float min_y = 60.0;
float min_z = 26.0;

/*
	设置四个关节的长度
	单位1mm
*/
float offset0 = -10;
float offset1 = -10;
float offset3 = -5;
float offset2 = -15;
void setup_kinematics(float L0, float L1, float L2, float L3, kinematics_t *kinematics) {
	//放大10倍
	kinematics->L0 = L0*10;
	kinematics->L1 = L1*10;
	kinematics->L2 = L2*10;
	kinematics->L3 = L3*10;
}

/*
	x,y 为映射到平面的坐标
	z为距离地面的距离
	Alpha 为爪子和平面的夹角 -25~-65范围比较好
*/

int kinematics_analysis(float x, float y, float z, float Alpha, kinematics_t *kinematics) {
	float theta3, theta4, theta5, theta6;
	float l0, l1, l2, l3;
	float aaa, bbb, ccc, zf_flag;
	
	//放大10倍
	x = x*10;
	y = y*10;
	z = z*10;
	
	
	l0 = kinematics->L0;
	l1 = kinematics->L1;
	l2 = kinematics->L2;
	l3 = kinematics->L3;
	
	if(x == 0) {
		theta6 = 0.0;
	} else {
		theta6 = atan(x/y)*180.0/pi;
	}
	
	y = sqrt(x*x + y*y);//x,y???    
    y = y-l3 * cos(Alpha*pi/180.0);  
    z = z-l0-l3*sin(Alpha*pi/180.0); 
    if(z < -l0) {
        return 1;
	}
    if(sqrt(y*y + z*z) > (l1+l2)) {
        return 2;
	}
	
	ccc = acos(y / sqrt(y * y + z * z));
    bbb = (y*y+z*z+l1*l1-l2*l2)/(2*l1*sqrt(y*y+z*z));
    if(bbb > 1 || bbb < -1) {
        return 5;
	}
    if (z < 0) {
        zf_flag = -1;
	} else {
        zf_flag = 1;
	}
    theta5 = ccc * zf_flag + acos(bbb);
    theta5 = theta5 * 180.0 / pi;
    if(theta5 > 180.0 || theta5 < 0.0) {
        return 6;
	}
	
    aaa = -(y*y+z*z-l1*l1-l2*l2)/(2*l1*l2);
    if (aaa > 1 || aaa < -1) {
        return 3;
	}
    theta4 = acos(aaa); 
    theta4 = 180.0 - theta4 * 180.0 / pi ;  
    if (theta4 > 135.0 || theta4 < -135.0) {
        return 4;
	}

    theta3 = Alpha - theta5 + theta4;
    if(theta3 > 90.0 || theta3 < -90.0) {
        return 7;
	}
	
	kinematics->servo_angle[0] = theta6;
	kinematics->servo_angle[1] = theta5-90;
	kinematics->servo_angle[2] = theta4;
	kinematics->servo_angle[3] = theta3;    
	
	kinematics->servo_pwm[0] = (int)(1500-2000.0 * kinematics->servo_angle[0] / 270.0);
	kinematics->servo_pwm[1] = (int)(1500+2000.0 * kinematics->servo_angle[1] / 270.0);
	kinematics->servo_pwm[2] = (int)(1500+2000.0 * kinematics->servo_angle[2] / 270.0);
	kinematics->servo_pwm[3] = (int)(1500-2000.0 * kinematics->servo_angle[3] / 270.0);

	return 0;
}

void caculate(float x, float y,float z)
{
	float L0 = 0.0f; 
	float L1 = 0.0f; //杆长
	float L2 = 0.0f;
	float L3 = 0.0f;
	float R = 0, D = 0;
	float aa,bb,cc,dd;
	R = sqrt(x*x+y*y);
	L0 = kinematics.L0;
	L1 = kinematics.L1;
	L2 = kinematics.L2;
	L3 = kinematics.L3;	
	D = sqrt((L0*L0)+(L1*L1)-2*L1*L2*cos60);
	if(R<=(L2 + D)) 
	{
		aa = 90.0f - acosf((R*R+D*D-L2*L2)/(2.0f*D*R)) * 57.295779513 - acosf((L0*L0+D*D-L1*L1)/(2*D*L0)) * 57.295779513;
		bb = 120.0f;
		cc = 180.0f - acosf((L1*L1+D*D-L0*L0)/(2.0f*D*L1)) * 57.295779513 - acosf((D*D+L2*L2-R*R)/(2.0f*D*L2)) * 57.295779513;
	}
	else if(R<=(D - L2)) 
	{
		aa = 30.0f;
		bb = 120.0f;
		cc = 120.0f;
	}
	else 
	{
		aa = 90.0f - acosf((L0*L0+(L2-R)*(L2-R)-L1*L1)/(2.0f*(R-L2)*L0)) * 57.295779513;
		bb = 180.0f - acosf((L0*L0-(L2-R)*(L2-R)+L1*L1)/(2.0f*L1*L0)) * 57.295779513;
		cc = 0.0f - acosf((L1*L1+(L2-R)*(L2-R)-L0*L0)/(2.0f*(R-L2)*L1)) * 57.295779513;
	}
	kinematics.servo_angle[0] = atanf(x/y) * 57.295779513;
	kinematics.servo_angle[1] = aa - z * 10;
	kinematics.servo_angle[2] = bb;
	kinematics.servo_angle[3] = cc; 
	
//	kinematics.servo_angle[0] = 0;
//	kinematics.servo_angle[1] = 0;
//	kinematics.servo_angle[2] = 0;
//	kinematics.servo_angle[3] = 0; 
	
	kinematics.servo_pwm[0] = (int)(1500 - 2000.0 * kinematics.servo_angle[0] / 270.0);
	kinematics.servo_pwm[1] = (int)(1500 + 2000.0 * kinematics.servo_angle[1] / 270.0);
	kinematics.servo_pwm[2] = (int)(1500 - 2000.0 * kinematics.servo_angle[2] / 270.0);
	kinematics.servo_pwm[3] = (int)(1500 + 2000.0 * kinematics.servo_angle[3] / 270.0);
}

void kinematics_analysisa(float x, float y,float z)
{
	float L0 = 0.0f; 
	float L1 = 0.0f; //杆长
	float L2 = 0.0f;
	float L3 = 0.0f;
	float R = 0, D = 0;
	float z0 = 0, r0 = 0;
	float a,b,c,d;
	R = sqrt(x*x+y*y);
	L0 = kinematics.L0 / 10;
	L1 = kinematics.L1 / 10;
	L2 = kinematics.L2 / 10;
	L3 = kinematics.L3 / 10;	
	z0=z-L0;
	r0=R-L3;
	D = sqrt((r0*r0)+(z0*z0));
	
	if(z>L0 + min_z){
		if(R>(L1+L2+L3)){
			kinematics.servo_pwm[0] = (int)(1500);
			kinematics.servo_pwm[1] = (int)(1500);
			kinematics.servo_pwm[2] = (int)(1500);
			kinematics.servo_pwm[3] = (int)(1500);
		}
		else{
			a = 90.0f - acosf((r0*r0+D*D-z0*z0)/(2.0f*D*r0)) * 57.295779513 - acosf((L1*L1+D*D-L2*L2)/(2*D*L1)) * 57.295779513;
			b = 180.0f - acosf((L1*L1+L2*L2-D*D)/(2.0f*L1*L2)) * 57.295779513;
			c = 90.0f - acosf((L2*L2+D*D-L1*L1)/(2.0f*D*L2)) * 57.295779513 - acosf((D*D+z0*z0-r0*r0)/(2.0f*D*z0)) * 57.295779513;
		}
	}
	else{
		kinematics.servo_pwm[0] = (int)(1500);
		kinematics.servo_pwm[1] = (int)(1500);
		kinematics.servo_pwm[2] = (int)(1500);
		kinematics.servo_pwm[3] = (int)(1500);
	}
	if(b < 5){
		kinematics.servo_pwm[0] = (int)(1500);
		kinematics.servo_pwm[1] = (int)(1500);
		kinematics.servo_pwm[2] = (int)(1500);
		kinematics.servo_pwm[3] = (int)(1500);
	}
	if(a > 85){
		kinematics.servo_pwm[0] = (int)(1500);
		kinematics.servo_pwm[1] = (int)(1500);
		kinematics.servo_pwm[2] = (int)(1500);
		kinematics.servo_pwm[3] = (int)(1500);
	}
	if(b > 133){
		kinematics.servo_pwm[0] = (int)(1500);
		kinematics.servo_pwm[1] = (int)(1500);
		kinematics.servo_pwm[2] = (int)(1500);
		kinematics.servo_pwm[3] = (int)(1500);
	}
	if(L1*sin(a/57.295779513) + L2 * cos((a + b - 90)/57.295779513) <= min_y){
		kinematics.servo_pwm[0] = (int)(1500);
		kinematics.servo_pwm[1] = (int)(1500);
		kinematics.servo_pwm[2] = (int)(1500);
		kinematics.servo_pwm[3] = (int)(1500);
	}
	
	kinematics.servo_angle[0] = atanf(x/y) * 57.295779513;
	kinematics.servo_angle[1] = a;
	kinematics.servo_angle[2] = b;
	kinematics.servo_angle[3] = c; 
	
	kinematics.servo_pwm[0] = (int)(1500 + offset0 - 2000.0 * kinematics.servo_angle[0] / 270.0);
	kinematics.servo_pwm[1] = (int)(1500 + offset1 - 2000.0 * kinematics.servo_angle[1] / 270.0);
	kinematics.servo_pwm[2] = (int)(1500 + offset2 + 2000.0 * kinematics.servo_angle[2] / 270.0);
	kinematics.servo_pwm[3] = (int)(1500 + offset3 + 2000.0 * kinematics.servo_angle[3] / 270.0);
}
