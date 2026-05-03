#ifndef FSM_H
#define FSM_H

#include <stdint.h>

void FSM_Init(void);
void FSM_Update(void);

uint8_t FSM_IsRunning(void);
uint8_t FSM_GetTurnCount(void);
const char *FSM_GetTurnSequence(void);

#endif
