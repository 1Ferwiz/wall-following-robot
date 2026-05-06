#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include "../config/pins.h"

/* ══════════════════════════════════════════════
 *  motor.h — TB6612FNG motor driver
 *  Team 11 Wall Following Robot
 *
 *  Controls 2× TT gear motors via TB6612FNG.
 *  Speed range: -255 (full reverse) to +255 (full forward)
 *  PWM frequency ≈ 7.8 kHz (Timer0, Fast PWM, /8 prescaler)
 * ══════════════════════════════════════════════ */

/* Base speeds — tune during testing */
#define MOTOR_BASE_SPEED        160    /* straight-line speed (0–255)   */
#define MOTOR_TURN_SPEED        140    /* speed during a 90° turn       */
#define MOTOR_SLOW_SPEED         80    /* approach speed near front wall */

/* Time-based 90° turn duration in ms — MUST BE CALIBRATED ON REAL ROBOT
 * Measure: run Motor_TurnLeft() and time until 90° reached, set here.  */
#define MOTOR_TURN_DURATION_MS  550U

/* ──────────────────────────────────────────
 *  Agreed Team Interface
 * ────────────────────────────────────────── */

/* Call once in main() — configures Timer0 and all direction pins */
void Motor_Init(void);

/* Set individual wheel speeds.
 * left / right: -255 (full reverse) to +255 (full forward)
 * 0 = brake (IN1=H, IN2=H)                                     */
void Motor_SetSpeed(int left, int right);

/* Immediately brakes both motors (IN1=H, IN2=H, PWM=0) */
void Motor_Stop(void);

/* Convenience wrappers used by FSM */
void Motor_Forward(void);
void Motor_TurnLeft(void);
void Motor_TurnRight(void);
void Motor_SlowDown(void);

#endif /* MOTOR_H */