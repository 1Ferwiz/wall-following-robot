#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

void SysTick_Init(void);
uint32_t SysTick_GetMs(void);

#endif /* SYSTICK_H */
