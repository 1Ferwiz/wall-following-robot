#include "../inc/fsm.h"
#include "../config/pins.h"
#include "../inc/encoder.h"
#include "../inc/motor.h"
#include "../inc/ultrasonic.h"
#include "../inc/uart.h"
#include "../inc/systick.h"

#define WALL_TARGET_MM 150

static FSM_State_t state = FSM_IDLE;
static uint8_t turn_count = 0;
static char turn_seq[FSM_MAX_TURNS + 1u];
static uint32_t state_timer = 0;
static uint32_t open_area_timer = 0;
static uint32_t last_turn_done_ms = 0;
static uint8_t report_sent = 0;
static uint8_t switch_was_on = 0;

static void log_turn(char turn) {
    if (turn_count < FSM_MAX_TURNS) {
        turn_seq[turn_count] = turn;
        turn_count++;
        turn_seq[turn_count] = '\0';
    }
}

static void reset_run_state(void) {
    turn_count = 0;
    turn_seq[0] = '\0';
    open_area_timer = 0;
    last_turn_done_ms = 0;
    report_sent = 0;
    switch_was_on = 0;
}

static void start_switch_init(void) {
    START_SW_DDR &= (uint8_t)~(1 << START_SW_PIN);
    START_SW_PORT |= (1 << START_SW_PIN);
}

static uint8_t start_switch_is_on(void) {
    return (START_SW_PINREG & (1 << START_SW_PIN)) ? 0u : 1u;
}

static void enter_state(FSM_State_t new_state) {
    state = new_state;
    state_timer = SysTick_GetMs();

    if (new_state == FSM_COMPLETE) {
        report_sent = 0;
    }

    if (new_state == FSM_TURNING_LEFT || new_state == FSM_TURNING_RIGHT) {
        Encoder_Reset();
    }
}

static uint32_t state_elapsed(void) {
    return SysTick_GetMs() - state_timer;
}

static uint8_t finish_detected(uint16_t front) {
    if (turn_count == 0 || last_turn_done_ms == 0) {
        open_area_timer = 0;
        return 0;
    }

    uint32_t now = SysTick_GetMs();
    if ((now - last_turn_done_ms) < FSM_FINISH_ARM_MS) {
        open_area_timer = 0;
        return 0;
    }

    uint16_t left = Ultrasonic_GetDistance(US_LEFT);
    uint16_t right = Ultrasonic_GetDistance(US_RIGHT);
    uint8_t front_open = (front == US_NO_ECHO || front > FSM_OPEN_SIDE_MM);
    uint8_t left_open = (left == US_NO_ECHO || left > FSM_OPEN_SIDE_MM);
    uint8_t right_open = (right == US_NO_ECHO || right > FSM_OPEN_SIDE_MM);

    if (front_open && left_open && right_open) {
        if (open_area_timer == 0) {
            open_area_timer = now;
        }
        return ((now - open_area_timer) >= FSM_FINISH_OPEN_MS) ? 1u : 0u;
    }

    open_area_timer = 0;
    return 0;
}

static void follow_walls(void) {
    uint16_t left = Ultrasonic_GetDistance(US_LEFT);
    uint16_t right = Ultrasonic_GetDistance(US_RIGHT);

    uint8_t l_valid = (left != US_NO_ECHO && left < 500u);
    uint8_t r_valid = (right != US_NO_ECHO && right < 500u);
    int error = 0;

    if (l_valid && r_valid) {
        error = (int)left - (int)right;
    } else if (l_valid) {
        error = (int)left - WALL_TARGET_MM;
    } else if (r_valid) {
        error = WALL_TARGET_MM - (int)right;
    }

    int correction = (FSM_KP * error) / 10;
    if (correction > 60) correction = 60;
    if (correction < -60) correction = -60;

    Motor_SetSpeed(MOTOR_BASE_SPEED - correction,
                   MOTOR_BASE_SPEED + correction);
}

void FSM_Init(void) {
    start_switch_init();
    reset_run_state();
    enter_state(FSM_IDLE);
}

void FSM_Update(void) {
    uint8_t switch_on = start_switch_is_on();

    if (!switch_on) {
        Motor_Stop();
        if (state != FSM_IDLE || turn_count != 0 || report_sent) {
            reset_run_state();
            enter_state(FSM_IDLE);
        }
        return;
    }

    if (!switch_was_on) {
        switch_was_on = 1;
        enter_state(FSM_IDLE);
    }

    uint16_t front = Ultrasonic_GetDistance(US_FRONT);

    switch (state) {
        case FSM_IDLE:
            Motor_Stop();
            if (state_elapsed() >= FSM_START_DELAY_MS) {
                enter_state(FSM_FOLLOW);
            }
            break;

        case FSM_FOLLOW:
            if (finish_detected(front)) {
                Motor_Stop();
                enter_state(FSM_COMPLETE);
            } else if (front != US_NO_ECHO && front < FSM_FRONT_TURN_MM) {
                Motor_Stop();
                enter_state(FSM_DETECT_TURN);
            } else if (front != US_NO_ECHO && front < FSM_FRONT_SLOW_MM) {
                Motor_SlowDown();
            } else {
                follow_walls();
            }
            break;

        case FSM_APPROACH_TURN:
            enter_state(FSM_DETECT_TURN);
            break;

        case FSM_DETECT_TURN: {
            if (state_elapsed() < FSM_SETTLE_MS) {
                Motor_Stop();
                break;
            }

            uint16_t left = Ultrasonic_GetDistance(US_LEFT);
            uint16_t right = Ultrasonic_GetDistance(US_RIGHT);
            uint8_t left_open = (left == US_NO_ECHO || left > FSM_OPEN_SIDE_MM);
            uint8_t right_open = (right == US_NO_ECHO || right > FSM_OPEN_SIDE_MM);

            if (left_open) {
                log_turn('L');
                enter_state(FSM_TURNING_LEFT);
            } else if (right_open) {
                log_turn('R');
                enter_state(FSM_TURNING_RIGHT);
            } else {
                log_turn('L');
                enter_state(FSM_TURNING_LEFT);
            }
            break;
        }

        case FSM_TURNING_LEFT:
            Motor_TurnLeft();
            if (state_elapsed() >= MOTOR_TURN_DURATION_MS) {
                Motor_Stop();
                last_turn_done_ms = SysTick_GetMs();
                open_area_timer = 0;
                enter_state(FSM_FOLLOW);
            }
            break;

        case FSM_TURNING_RIGHT:
            Motor_TurnRight();
            if (state_elapsed() >= MOTOR_TURN_DURATION_MS) {
                Motor_Stop();
                last_turn_done_ms = SysTick_GetMs();
                open_area_timer = 0;
                enter_state(FSM_FOLLOW);
            }
            break;

        case FSM_COMPLETE:
            Motor_Stop();
            if (!report_sent) {
                UART_SendTurnReport(turn_count, turn_seq);
                report_sent = 1;
            }
            break;
    }
}

FSM_State_t FSM_GetState(void) { return state; }
uint8_t FSM_GetTurnCount(void) { return turn_count; }
uint8_t FSM_IsRunning(void) { return (state != FSM_IDLE && state != FSM_COMPLETE); }
const char *FSM_GetTurnSequence(void) { return turn_seq; }
