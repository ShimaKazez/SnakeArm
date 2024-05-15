/*
 * FOC.h
 *
 *  Created on: Mar 5, 2024
 *      Author: Cross
 */

#include "main.h"
#include "math.h"
#include "arm_math.h"
#include "tim.h"
#include "adc.h"

#ifndef FOC_FOC_H_
#define FOC_FOC_H_

#define CKTIM 170000000//定时器时钟频率
#define PWM_PRSC 0
#define PWM_FREQ 15000//PWM频率
#define PWM_PERIOD CKTIM/(2*PWM_FREQ*(PWM_PRSC+1))
#define REP_RATE 1 //电流环刷新频率为(REP_RATE+1)/(2*PWM_FREQ)
#define DEADTIME_NS 1000//死区时间ns
#define DEADTIME CKTIM/1000000/2*DEADTIME_NS/1000
#define POLE_PAIR_NUM 4//极对数
#define ENCODER_PPR 1250//编码器线数
#define ALIGNMENT_ANGLE 300
#define COUNTER_RESET (ALIGNMENT_ANGLE*4*ENCODER_PPR/360-1)/POLE_PAIR_NUM
#define ICx_FILTER 8

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef __IO uint32_t vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t vu8;

#define U8_MAX     ((u8)255)
#define S8_MAX     ((s8)127)
#define S8_MIN     ((s8)-128)
#define U16_MAX    ((u16)65535u)
#define S16_MAX    ((s16)32767)
#define S16_MIN    ((s16)-32768)
#define U32_MAX    ((u32)4294967295uL)
#define S32_MAX    ((s32)2147483647)
#define S32_MIN    ((s32)-2147483648)

//结构体定义
typedef struct {
	s16 qI_Component1;
	s16 qI_Component2;
} Curr_Components;

typedef struct {
	s16 qV_Component1;
	s16 qV_Component2;
} Volt_Components;

typedef struct     //电压值结构体
{
	s16 hCos;
	s16 hSin;
} Trig_Components;  //存放角度sin和cos函数值的结构体

typedef struct {
	s16 hKp_Gain;			   //比例系数
	u16 hKp_Divisor;		   //比例系数因子
	s16 hKi_Gain;		       //积分系数
	u16 hKi_Divisor;  	       //积分系数因子
	s16 hLower_Limit_Output;   //总输出下限
	s16 hUpper_Limit_Output;   //总输出上限
	s32 wLower_Limit_Integral; //积分项下限
	s32 wUpper_Limit_Integral; //积分项上限
	s32 wIntegral;			   //积分累积和
	s16 hKd_Gain;			   //微分系数
	u16 hKd_Divisor;		   //微分系数因子
	s32 wPreviousError;	       //上次误差
} PID_Struct_t;

//数学变换部分
#define S16_MAX    ((s16)32767)
#define S16_MIN    ((s16)-32768)
#define divSQRT_3	(s16)0x49E6      //1/sqrt(3)的Q15格式，1/sqrt(3)*2^15=18918=0x49E6
#define SIN_MASK  0x0300
#define U0_90     0x0200
#define U90_180   0x0300
#define U180_270  0x0000
#define U270_360  0x0100
#define SQRT_3		1.732051
#define T		    (PWM_PERIOD * 4)
#define T_SQRT3     (u16)(T * SQRT_3)
//SVPWM部分
#define SECTOR_1	(u32)1
#define SECTOR_2	(u32)2
#define SECTOR_3	(u32)3
#define SECTOR_4	(u32)4
#define SECTOR_5	(u32)5
#define SECTOR_6	(u32)6
#define PWM2_MODE 0
#define PWM1_MODE 1
#define TW_AFTER ((u16)(((DEADTIME_NS+MAX_TNTR_NS)*168uL)/1000ul))
#define TW_BEFORE (((u16)(((((u16)(SAMPLING_TIME_NS)))*168uL)/1000ul))+1)
#define TNOISE_NS 1550     //2.55usec
#define TRISE_NS 1550     //2.55usec
#define SAMPLING_TIME_NS   700  //700ns
#define SAMPLING_TIME (u16)(((u16)(SAMPLING_TIME_NS) * 168uL)/1000uL)
#define TNOISE (u16)((((u16)(TNOISE_NS)) * 168uL)/1000uL)
#define TRISE (u16)((((u16)(TRISE_NS)) * 168uL)/1000uL)
#define TDEAD (u16)((DEADTIME_NS * 168uL)/1000uL)

#if (TNOISE_NS > TRISE_NS)
  #define MAX_TNTR_NS TNOISE_NS
#else
#define MAX_TNTR_NS TRISE_NS
#endif

//函数声明

//数学变换
Curr_Components Clarke(Curr_Components Curr_Input);
Trig_Components Trig_Functions(s16 hAngle);
Curr_Components Park(Curr_Components Curr_Input, s16 Theta);
Volt_Components Rev_Park(Volt_Components Volt_Input);
//SVPWM
void SVPWM_3ShuntCalcDutyCycles(Volt_Components Stat_Volt_Input);
//FOC核心
void FOC_Control(void);
//系统初始化
void motor_init(void);

#endif /* FOC_FOC_H_ */
