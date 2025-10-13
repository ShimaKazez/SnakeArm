#ifndef ERROR_H_
#define ERROR_H_

#include "UI.h"
#include "ADC_Sample.h"

// 定义联合体
typedef union {
	uint16_t all; // 整体访问 16 位错误代码
	struct {
		uint16_t WARNING_TENSION_OUT :1; // Bit 0
		uint16_t WARNING_TIME_OUT :1; // Bit 1
		uint16_t WARNING_VOLTAGE_OUT :1; // Bit 2
		uint16_t WARNING_CURRENT_OUT :1; // Bit 3
		uint16_t WARNING_4 :1; // Reserved (Bit 4)
		uint16_t WARNING_5 :1; // Reserved (Bit 5)
		uint16_t WARNING_6 :1; // Reserved (Bit 6)
		uint16_t WARNING_7 :1; // Reserved (Bit 7)
		uint16_t ERROR_TIMEOUT :1; // Bit 8
		uint16_t ERROR_TORQUE_OUT :1; // Bit 9
		uint16_t ERROR_SIGNAL_LOST :1; // Bit 10
		uint16_t ERROR_ZERO_OUT :1; // Bit 11
		uint16_t ERROR_4 :1; // Reserved (Bit 12)
		uint16_t ERROR_5 :1; // Reserved (Bit 13)
		uint16_t ERROR_6 :1; // Reserved (Bit 14)
		uint16_t ERROR_7 :1; // Reserved (Bit 15)
	} bits; // 按位访问
} ErrorCodeUnion;

#define MIN_VOLTAGE 0.0
#define MAX_VOLTAGE 28.0
#define MAX_CURRENT 8.0
#define MIN_TENSION -1.0
#define MAX_TENSION 100.0
#define MAX_TORQUE 2.4

// 函数声明
void HandleError(uint16_t);
void HandleWarning(uint16_t);
void CheckAndHandleErrors(void);

#endif /* ERROR_H_ */
