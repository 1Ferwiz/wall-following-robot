#include "fsm.h"

typedef enum {
    IDLE,
    FOLLOW_WALL,
    TURN_LEFT,
    TURN_RIGHT,
    AVOID,
    SEARCH,
    FINISHED
} State_t;

static State_t currentState;

void FSM_Init(void)
{
    currentState = IDLE;
}

void FSM_Update(void)
{
    switch (currentState)
    {
        case IDLE:
            // wait for start
            break;

        case FOLLOW_WALL:
            // read sensors
            // apply PID
            // check turns
            break;

        case TURN_LEFT:
            // rotate left
            break;

        case TURN_RIGHT:
            // rotate right
            break;

        case AVOID:
            // collision handling
            break;

        case SEARCH:
            // find wall again
            break;

        case FINISHED:
            // send data via UART
            break;
    }
}