#ifndef FSM_H
#define FSM_H

#include <stdint.h>

#define FSM_MAX_TURNS          32u
#define FSM_KP                  3
#define FSM_FRONT_SLOW_MM     280u
#define FSM_FRONT_TURN_MM     180u
#define FSM_OPEN_SIDE_MM      300u
#define FSM_SETTLE_MS          80u
#define FSM_FINISH_OPEN_MS    250u
#define FSM_START_DELAY_MS    500u

typedef enum {
    FSM_IDLE = 0,
    FSM_FOLLOW,
    FSM_APPROACH_TURN,
    FSM_DETECT_TURN,
    FSM_TURNING_LEFT,
    FSM_TURNING_RIGHT,
    FSM_COMPLETE
} FSM_State_t;

void FSM_Init(void);
void FSM_Update(void);

uint8_t FSM_IsRunning(void);
FSM_State_t FSM_GetState(void);
uint8_t FSM_GetTurnCount(void);
const char *FSM_GetTurnSequence(void);

#endif
