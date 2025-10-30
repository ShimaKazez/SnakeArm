#ifndef ERROR_H_
#define ERROR_H_

#include "UI.h"

// 定义联合体
typedef union {
	uint16_t all; // 整体访问 16 位错误代码
	struct {
		uint16_t WARNING_VOLTAGE_OUT :1; // Bit 0
		uint16_t WARNING_CURRENT_OUT :1; // Bit 1
		uint16_t WARNING_TIME_OUT :1; // Bit 2
		uint16_t WARNING_3 :1; // Bit 3

		uint16_t WARNING_TENSION_OUT :1; // Reserved (Bit 4)
		uint16_t WARNING_ANGLE_OUT :1; // Reserved (Bit 5)
		uint16_t WARNING_SPEED_OUT :1; // Reserved (Bit 6)
		uint16_t WARNING_TORQUE_OUT :1; // Reserved (Bit 7)

		uint16_t ERROR_SIGNAL_LOST :1; // Bit 8
		uint16_t ERROR_TIMEOUT :1; // Bit 9
		uint16_t ERROR_10 :1; // Bit 10
		uint16_t ERROR_11 :1; // Bit 11

		uint16_t ERROR_TENSION_OUT :1; // Reserved (Bit 12)
		uint16_t ERROR_ANGLE_OUT :1; // Reserved (Bit 13)
		uint16_t ERROR_SPEED_OUT :1; // Reserved (Bit 14)
		uint16_t ERROR_TORQUE_OUT :1; // Reserved (Bit 15)
	} bits; // 按位访问
} ErrorCodeUnion;
extern volatile ErrorCodeUnion ErrorCode_Sys; // 全局错误代码变量

// 错误和警告计数器上限
#define COUNTER_LIMIT 50

// 监控变量结构体
typedef struct {
	float variable;            // 被监控的变量
	float error_low_limit;     // 错误下限
	float error_high_limit;    // 错误上限
	float warn_low_limit;      // 警告下限
	float warn_high_limit;     // 警告上限
	uint16_t error_counter;    // 错误计数器
	uint16_t warn_counter;     // 警告计数器
	uint16_t error_bitmask;    // 错误位掩码
	uint16_t warn_bitmask;     // 警告位掩码
} StatusMonitor;
extern StatusMonitor Tension_Monitors[3];
extern StatusMonitor Angle_Monitors[3];
extern StatusMonitor Speed_Monitors[3];
extern StatusMonitor Torque_Monitors[3];
extern StatusMonitor Voltage_Monitor, Current_Monitor, Timeout_Monitor;
uint16_t Status_monitor(StatusMonitor*, float);
void Status_monitor_init_all(void);

// 函数声明
void HandleError(uint16_t);
void HandleWarning(uint16_t);
void ClearError(uint16_t);

#endif /* ERROR_H_ */
