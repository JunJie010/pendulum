#ifndef _PID_H
#define _PID_H

typedef struct {
	float SP;
	float PV;
	float MV;
	
	float Kp;
	float Ki;
	float Kd;
	
	float Error0;
	float Error1;
	float ErrorInt;
	
	float MVmax;
	float MVmin;
} PID;         //定义PID结构体类型

void PID_Move(PID *p);

#endif
