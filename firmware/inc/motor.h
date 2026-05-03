#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include "pins.h"

/*
 * motor.h — TB6612FNG dual motor driver interface
 * Register-level AVR only, no Arduino libraries.
 * All pin definitions come from pins.h.
 *
 * Timer0 Fast PWM:
 *   OC0B → MOTOR_L_PWM_PIN (PD5) — Left  motor
 *   OC0A → MOTOR_R_PWM_PIN (PD6) — Right motor
 */

/* ── Constants ─────────────────────────────────────────────────────────── */

#define MOTOR_PWM_MAX     255
#define MOTOR_PWM_MIN     0
#define MOTOR_BASE_SPEED  160   /* tune experimentally */

/* ── Lifecycle ─────────────────────────────────────────────────────────── */

/* Initialise GPIO and Timer0 for PWM, leave motors stopped */
void motor_init(void);

/* STBY pin control — call motor_enable() after motor_init() */
void motor_enable(void);    /* STBY HIGH — TB6612 active  */
void motor_disable(void);   /* STBY LOW  — TB6612 standby */

/* ── FSM motion commands ───────────────────────────────────────────────── */

void motor_forward(uint8_t speed);      /* both wheels forward             */
void motor_backward(uint8_t speed);     /* both wheels backward            */
void motor_stop(void);                  /* coast stop  — PWM = 0          */
void motor_brake(void);                 /* active brake — use before turns */
void motor_turn_left(uint8_t speed);    /* in-place left  rotation         */
void motor_turn_right(uint8_t speed);   /* in-place right rotation         */

/* ── PID wall-following ────────────────────────────────────────────────── */

/* Signed PWM, -255 to +255. Negative = reverse that wheel.
 * Called every 20 ms by the PID control loop. */
void motor_set_differential(int16_t left_pwm, int16_t right_pwm);

/* ── Individual motor control ──────────────────────────────────────────── */

/* speed: 0-255, dir: 1 = forward, 0 = backward */
void motor_left_set(uint8_t speed, uint8_t dir);
void motor_right_set(uint8_t speed, uint8_t dir);

/* ── README compatibility wrappers ────────────────────────────────────── */

/* Thin wrappers so the agreed README interface is not broken */
void Motor_SetSpeed(int left, int right);   /* → motor_set_differential() */
void Motor_Stop(void);                      /* → motor_stop()             */

#endif /* MOTOR_H */
