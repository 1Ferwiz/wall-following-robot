#include "../inc/fsm.h"
#include "../inc/encoder.h"
#include "../inc/motor.h"
#include "../inc/ultrasonic.h"
#include "../inc/uart.h"
#include "../inc/systick.h"

/* ══════════════════════════════════════════════
 *  fsm.c — Wall Following State Machine
 *
 *  Wall following algorithm:
 *   → If both side walls visible: centre between them
 *   → If only left wall: maintain target distance from it
 *   → If only right wall: maintain target distance from it
 *
 *  Correction formula (proportional only):
 *   error      = left_dist - right_dist   (balance mode)
 *             OR left_dist  - TARGET       (left-only mode)
 *             OR TARGET     - right_dist   (right-only mode)
 *
 *   correction = (FSM_KP * error) / 10
 *   left_spd   = BASE - correction
 *   right_spd  = BASE + correction
 *
 *  Turn detection:
 *   Front < FSM_FRONT_SLOW_MM  → slow down
 *   Front < FSM_FRONT_TURN_MM  → stop, check which side is open
 *   Open side → that is the turn direction
 * ══════════════════════════════════════════════ */

/* Target side wall distance in mm */
#define WALL_TARGET_MM    150

/* ──────────────────────────────────────────
 *  Private state
 * ────────────────────────────────────────── */
static FSM_State_t  state       = FSM_IDLE;
static uint8_t      turn_count  = 0;
static char         turn_seq[FSM_MAX_TURNS + 1u];  /* 'L' or 'R' per turn */
static uint32_t     state_timer = 0;          /* ms when state was entered */
static uint8_t      report_sent = 0;
static uint32_t     open_area_timer = 0;

/* ──────────────────────────────────────────
 *  Private helper: enter a new state
 *  Records the entry time for timeout logic
 * ────────────────────────────────────────── */
static void enter_state(FSM_State_t new_state) {
    state       = new_state;
    state_timer = SysTick_GetMs();

    if (new_state == FSM_COMPLETE) {
        report_sent = 0;
    }

    if (new_state == FSM_TURNING_LEFT || new_state == FSM_TURNING_RIGHT) {
        Encoder_Reset();
    }
}

/* How many ms have elapsed since we entered current state */
static uint32_t state_elapsed(void) {
    return SysTick_GetMs() - state_timer;
}

/* ──────────────────────────────────────────
 *  Private helper: wall following steering
 *  Returns corrected {left_speed, right_speed}
 *  by adjusting for wall distance error.
 * ────────────────────────────────────────── */
static void follow_walls(void) {
    uint16_t left  = Ultrasonic_GetDistance(US_LEFT);
    uint16_t right = Ultrasonic_GetDistance(US_RIGHT);

    int error      = 0;
    uint8_t l_valid = (left  != US_NO_ECHO && left  < 500);
    uint8_t r_valid = (right != US_NO_ECHO && right < 500);

    if (l_valid && r_valid) {
        /* Both walls visible — balance in the middle */
        error = (int)left - (int)right;
    } else if (l_valid) {
        /* Left wall only — maintain target distance */
        error = (int)left - WALL_TARGET_MM;
    } else if (r_valid) {
        /* Right wall only — maintain target distance */
        error = WALL_TARGET_MM - (int)right;
    } else {
        /* No walls visible — go straight */
        error = 0;
    }

    /* Proportional correction (Kp = FSM_KP/10) */
    int correction = (FSM_KP * error) / 10;

    /* Clamp correction so we don't reverse a wheel */
    if (correction >  80) correction =  80;
    if (correction < -80) correction = -80;

    int left_spd  = MOTOR_BASE_SPEED - correction;
    int right_spd = MOTOR_BASE_SPEED + correction;

    Motor_SetSpeed(left_spd, right_spd);
}

/* ──────────────────────────────────────────
 *  FSM_Init
 * ────────────────────────────────────────── */
void FSM_Init(void) {
    turn_count = 0;
    turn_seq[0] = '\0';
    report_sent = 0;
    open_area_timer = 0;
    enter_state(FSM_IDLE);
}

/* ──────────────────────────────────────────
 *  FSM_Update  — call every main loop iteration
 * ────────────────────────────────────────── */
void FSM_Update(void) {

    uint16_t front = Ultrasonic_GetDistance(US_FRONT);

    switch (state) {

        /* ────────────────────────────────────────
         *  IDLE — motors off, waiting
         *  Transitions to FOLLOW automatically.
         *  Insert a start button / Bluetooth trigger here later.
         * ──────────────────────────────────────── */
        case FSM_IDLE:
            Motor_Stop();
            if (state_elapsed() > 500) {     /* 500ms startup delay */
                enter_state(FSM_FOLLOW);
            }
            break;

        /* ────────────────────────────────────────
         *  FOLLOW — main wall-following state
         *  Steers to maintain target wall distance.
         *  Exits when front wall detected.
         * ──────────────────────────────────────── */
        case FSM_FOLLOW:
            if (front < FSM_FRONT_SLOW_MM && front != US_NO_ECHO) {
                open_area_timer = 0;
                enter_state(FSM_APPROACH_TURN);
            } else {
                uint16_t left = Ultrasonic_GetDistance(US_LEFT);
                uint16_t right = Ultrasonic_GetDistance(US_RIGHT);
                uint8_t front_open = (front == US_NO_ECHO || front > FSM_OPEN_SIDE_MM);
                uint8_t left_open = (left == US_NO_ECHO || left > FSM_OPEN_SIDE_MM);
                uint8_t right_open = (right == US_NO_ECHO || right > FSM_OPEN_SIDE_MM);

                if (turn_count > 0 && front_open && left_open && right_open) {
                    if (open_area_timer == 0) {
                        open_area_timer = SysTick_GetMs();
                    } else if ((SysTick_GetMs() - open_area_timer) >= FSM_FINISH_OPEN_MS) {
                        Motor_Stop();
                        enter_state(FSM_COMPLETE);
                        break;
                    }
                } else {
                    open_area_timer = 0;
                }

                follow_walls();
            }
            break;

        /* ────────────────────────────────────────
         *  APPROACH_TURN — front wall getting close
         *  Slows down, waits until close enough to read turn.
         * ──────────────────────────────────────── */
        case FSM_APPROACH_TURN:
            Motor_SlowDown();
            if (front < FSM_FRONT_TURN_MM && front != US_NO_ECHO) {
                Motor_Stop();
                enter_state(FSM_DETECT_TURN);
            }
            /* Safety: if front reading disappears, go back to follow */
            if (front == US_NO_ECHO || front > FSM_FRONT_SLOW_MM) {
                enter_state(FSM_FOLLOW);
            }
            break;

        /* ────────────────────────────────────────
         *  DETECT_TURN — robot is stopped near wall
         *  Waits for sensors to settle, then reads
         *  which side is open to determine turn direction.
         * ──────────────────────────────────────── */
        case FSM_DETECT_TURN:
            /* Wait for settle time before reading sensors */
            if (state_elapsed() < FSM_SETTLE_MS) break;

            uint16_t left  = Ultrasonic_GetDistance(US_LEFT);
            uint16_t right = Ultrasonic_GetDistance(US_RIGHT);

            uint8_t left_open  = (left  > FSM_OPEN_SIDE_MM || left  == US_NO_ECHO);
            uint8_t right_open = (right > FSM_OPEN_SIDE_MM || right == US_NO_ECHO);

            if (left_open && !right_open) {
                /* Left is open → turn left */
                if (turn_count < FSM_MAX_TURNS) {
                    turn_seq[turn_count] = 'L';
                    turn_count++;
                    turn_seq[turn_count] = '\0';
                }
                enter_state(FSM_TURNING_LEFT);

            } else if (right_open && !left_open) {
                /* Right is open → turn right */
                if (turn_count < FSM_MAX_TURNS) {
                    turn_seq[turn_count] = 'R';
                    turn_count++;
                    turn_seq[turn_count] = '\0';
                }
                enter_state(FSM_TURNING_RIGHT);

            } else if (left_open && right_open) {
                /* Both open (T-junction or end) → default turn left
                 * Change this rule based on your track layout.       */
                if (turn_count < FSM_MAX_TURNS) {
                    turn_seq[turn_count] = 'L';
                    turn_count++;
                    turn_seq[turn_count] = '\0';
                }
                enter_state(FSM_TURNING_LEFT);

            } else {
                /* Dead end — track complete */
                Motor_Stop();
                enter_state(FSM_COMPLETE);
            }
            break;

        /* ────────────────────────────────────────
         *  TURNING_LEFT
         *  Runs left turn motors for MOTOR_TURN_DURATION_MS,
         *  then returns to FOLLOW.
         * ──────────────────────────────────────── */
        case FSM_TURNING_LEFT:
            Motor_TurnLeft();
            if (Encoder_TurnComplete(TICKS_FOR_90_TURN) ||
                state_elapsed() >= MOTOR_TURN_DURATION_MS) {
                Motor_Stop();
                enter_state(FSM_FOLLOW);
            }
            break;

        /* ────────────────────────────────────────
         *  TURNING_RIGHT
         * ──────────────────────────────────────── */
        case FSM_TURNING_RIGHT:
            Motor_TurnRight();
            if (Encoder_TurnComplete(TICKS_FOR_90_TURN) ||
                state_elapsed() >= MOTOR_TURN_DURATION_MS) {
                Motor_Stop();
                enter_state(FSM_FOLLOW);
            }
            break;

        /* ────────────────────────────────────────
         *  COMPLETE — track finished
         *  Sends turn report once via Bluetooth.
         * ──────────────────────────────────────── */
        case FSM_COMPLETE:
            Motor_Stop();
            if (!report_sent) {
                UART_SendTurnReport(turn_count, turn_seq);
                report_sent = 1;
            }
            break;
    }
}

/* ──────────────────────────────────────────
 *  Getters
 * ────────────────────────────────────────── */
FSM_State_t FSM_GetState(void)    { return state;      }
uint8_t     FSM_GetTurnCount(void){ return turn_count; }
uint8_t     FSM_IsRunning(void)   { return (state != FSM_IDLE && state != FSM_COMPLETE); }
const char *FSM_GetTurnSequence(void) { return turn_seq; }
