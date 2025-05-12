#ifndef ERROR_H_
#define ERROR_H_

#include "UI.h"

// 错误代码枚举
typedef enum {
    ERROR_NONE = 0,
    ERROR_TIMEOUT = 900,
    ERROR_TORQUE_OUT = 901,
    ERROR_SIGNAL_LOST = 902,
    ERROR_ZERO_OUT = 903,
    WARNING_TENSION_OUT = 801,
	WARNING_TIME_OUT = 802
} ErrorCode;

// 函数声明
void HandleError(ErrorCode code);
void HandleWarning(ErrorCode code);
void CheckAndHandleErrors(void);

#endif /* ERROR_H_ */
