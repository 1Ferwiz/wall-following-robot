#include "fsm.h"
#include <stdint.h>
#include <stdbool.h>

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
            // TODO: wait for start signal
            break;

        case FOLLOW_WALL:
            // TODO:
            // - read IR + ultrasonic
            // - apply PID
            // - detect turns
            break;

        case TURN_LEFT:
            // TODO: encoder-based rotation
            break;

        case TURN_RIGHT:
            break;

        case AVOID:
            // TODO: reverse + adjust
            break;

        case SEARCH:
            // TODO: find wall again
            break;

        case FINISHED:
            // TODO: send Bluetooth data
            break;
    }
}