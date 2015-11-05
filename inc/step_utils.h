#ifndef __ITSTEP_UTILS_H
#define __ITSTEP_UTILS_H

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include <stdint.h>
#include "ringbuffer.h"

#define FONT Font12

void RCC_Init();
void STEP_Init();
void STEP_ClrScr();
void STEP_Print(const char * str);
void STEP_Println(const char * str);
void STEP_DBG_Osc();

HAL_StatusTypeDef STEP_UART_Receive_IT(UART_HandleTypeDef *huart, RingBuffer *rBuffer);

#endif
