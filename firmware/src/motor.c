#include "motor.h"
#include <avr/io.h>
#include <stdint.h>

/*
 * motor.c — TB6612FNG dual motor driver
 * Register-level AVR only, no Arduino libraries.
 * All pin/port references go through pins.h macros.
 *
 * Timer0 Fast PWM (mode 3):
 *   OC0B → MOTOR_L_PWM_PIN (PD5) — Left  motor speed
 *   OC0A → MOTOR_R_PWM_PIN (PD6) — Right motor speed
 *   Prescaler 64 → ~976 Hz PWM (16 MHz / 64 / 256)
 *
 * TB6612 truth table per motor:
 *   IN1=1, IN2=0 → Forward
 *   IN1=0, IN2=1 → Backward
 *   IN1=1, IN2=1 → Brake
 *   IN1=0, IN2=0 → Coast
 */

/* ── Internal direction helpers ────────────────────────────────────────── */

static inline void _left_forward(void)
{
    MOTOR_DIR_PORT |=  (1 << MOTOR_L_IN1_PIN);
    MOTOR_DIR_PORT &= ~(1 << MOTOR_L_IN2_PIN);
}

static inline void _left_backward(void)
{
    MOTOR_DIR_PORT &= ~(1 << MOTOR_L_IN1_PIN);
    MOTOR_DIR_PORT |=  (1 << MOTOR_L_IN2_PIN);
}

static inline void _left_brake(void)
{
    MOTOR_DIR_PORT |= (1 << MOTOR_L_IN1_PIN) | (1 << MOTOR_L_IN2_PIN);
}

static inline void _left_coast(void)
{
    MOTOR_DIR_PORT &= ~((1 << MOTOR_L_IN1_PIN) | (1 << MOTOR_L_IN2_PIN));
}

static inline void _right_forward(void)
{
    MOTOR_DIR_PORT |=  (1 << MOTOR_R_IN1_PIN);
    MOTOR_DIR_PORT &= ~(1 << MOTOR_R_IN2_PIN);
}

static inline void _right_backward(void)
{
    MOTOR_DIR_PORT &= ~(1 << MOTOR_R_IN1_PIN);
    MOTOR_DIR_PORT |=  (1 << MOTOR_R_IN2_PIN);
}

static inline void _right_brake(void)
{
    MOTOR_DIR_PORT |= (1 << MOTOR_R_IN1_PIN) | (1 << MOTOR_R_IN2_PIN);
}

static inline void _right_coast(void)
{
    MOTOR_DIR_PORT &= ~((1 << MOTOR_R_IN1_PIN) | (1 << MOTOR_R_IN2_PIN));
}

/* ── Lifecycle ─────────────────────────────────────────────────────────── */

void motor_init(void)
{
    /* STBY as output, drive LOW (disabled) until motor_enable() is called */
    MOTOR_STBY_DDR  |=  (1 << MOTOR_STBY_PIN);
    MOTOR_STBY_PORT &= ~(1 << MOTOR_STBY_PIN);

    /* PWM pins as outputs */
    MOTOR_L_PWM_DDR |= (1 << MOTOR_L_PWM_PIN);
    MOTOR_R_PWM_DDR |= (1 << MOTOR_R_PWM_PIN);

    /* Direction pins as outputs */
    MOTOR_DIR_DDR |= (1 << MOTOR_L_IN1_PIN) | (1 << MOTOR_L_IN2_PIN)
                   | (1 << MOTOR_R_IN1_PIN) | (1 << MOTOR_R_IN2_PIN);

    /* Coast both motors */
    _left_coast();
    _right_coast();

    /*
     * Timer0 Fast PWM mode 3: WGM01=1, WGM00=1
     * OC0A non-inverting: COM0A1=1 → Right motor (PD6)
     * OC0B non-inverting: COM0B1=1 → Left  motor (PD5)
     * Prescaler 64: CS01=1, CS00=1
     */
    TCCR0A = (1 << COM0A1) | (1 << COM0B1)
           | (1 << WGM01)  | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);

    /* Zero duty cycle */
    OCR0A = 0;   /* Right motor */
    OCR0B = 0;   /* Left  motor */
}

void motor_enable(void)
{
    MOTOR_STBY_PORT |= (1 << MOTOR_STBY_PIN);
}

void motor_disable(void)
{
    MOTOR_STBY_PORT &= ~(1 << MOTOR_STBY_PIN);
}

/* ── Individual motor control ──────────────────────────────────────────── */

void motor_left_set(uint8_t speed, uint8_t dir)
{
    if (dir) { _left_forward(); } else { _left_backward(); }
    OCR0B = speed;
}

void motor_right_set(uint8_t speed, uint8_t dir)
{
    if (dir) { _right_forward(); } else { _right_backward(); }
    OCR0A = speed;
}

/* ── FSM motion commands ───────────────────────────────────────────────── */

void motor_forward(uint8_t speed)
{
    _left_forward();
    _right_forward();
    OCR0B = speed;
    OCR0A = speed;
}

void motor_backward(uint8_t speed)
{
    _left_backward();
    _right_backward();
    OCR0B = speed;
    OCR0A = speed;
}

void motor_stop(void)
{
    OCR0B = 0;
    OCR0A = 0;
    _left_coast();
    _right_coast();
}

void motor_brake(void)
{
    OCR0B = 0;
    OCR0A = 0;
    _left_brake();
    _right_brake();
}

void motor_turn_left(uint8_t speed)
{
    /* Left wheel backward, right wheel forward → spins left in place */
    _left_backward();
    _right_forward();
    OCR0B = speed;
    OCR0A = speed;
}

void motor_turn_right(uint8_t speed)
{
    /* Left wheel forward, right wheel backward → spins right in place */
    _left_forward();
    _right_backward();
    OCR0B = speed;
    OCR0A = speed;
}

/* ── PID wall-following ────────────────────────────────────────────────── */

void motor_set_differential(int16_t left_pwm, int16_t right_pwm)
{
    /* Left motor → OC0B */
    if (left_pwm >= 0) {
        _left_forward();
        OCR0B = (left_pwm > MOTOR_PWM_MAX) ? MOTOR_PWM_MAX : (uint8_t)left_pwm;
    } else {
        _left_backward();
        int16_t mag = -left_pwm;
        OCR0B = (mag > MOTOR_PWM_MAX) ? MOTOR_PWM_MAX : (uint8_t)mag;
    }

    /* Right motor → OC0A */
    if (right_pwm >= 0) {
        _right_forward();
        OCR0A = (right_pwm > MOTOR_PWM_MAX) ? MOTOR_PWM_MAX : (uint8_t)right_pwm;
    } else {
        _right_backward();
        int16_t mag = -right_pwm;
        OCR0A = (mag > MOTOR_PWM_MAX) ? MOTOR_PWM_MAX : (uint8_t)mag;
    }
}

/* ── README compatibility wrappers ────────────────────────────────────── */

void Motor_SetSpeed(int left, int right)
{
    motor_set_differential((int16_t)left, (int16_t)right);
}

void Motor_Stop(void)
{
    motor_stop();
}
