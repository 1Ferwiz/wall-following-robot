#include "../inc/motor.h"
#include <avr/io.h>
#include <stdint.h>

/* ══════════════════════════════════════════════
 *  motor.c — TB6612FNG register-level driver
 *
 *  TB6612FNG truth table per channel:
 *  IN1 | IN2 | PWM | Result
 *   H  |  L  |  H  | Forward
 *   L  |  H  |  H  | Reverse
 *   H  |  H  |  x  | Brake (short brake)
 *   L  |  L  |  x  | Coast (free spin)
 *
 *  Timer0 — Fast PWM mode:
 *   WGM  = 011  → Fast PWM, TOP = 0xFF
 *   COM0A = 10  → non-inverting on OC0A (PD6, right motor)
 *   COM0B = 10  → non-inverting on OC0B (PD5, left  motor)
 *   CS0   = 010 → prescaler /8 → 16MHz/8/256 = 7812 Hz
 *
 *   OCR0A → right motor PWM duty (0–255)
 *   OCR0B → left  motor PWM duty (0–255)
 * ══════════════════════════════════════════════ */

/* ──────────────────────────────────────────
 *  Private helpers — direction pins only
 * ────────────────────────────────────────── */
static void set_left_forward(void) {
    MOTOR_DIR_PORT |=  (1 << MOTOR_L_IN1_PIN);
    MOTOR_DIR_PORT &= ~(1 << MOTOR_L_IN2_PIN);
}
static void set_left_reverse(void) {
    MOTOR_DIR_PORT &= ~(1 << MOTOR_L_IN1_PIN);
    MOTOR_DIR_PORT |=  (1 << MOTOR_L_IN2_PIN);
}
static void set_left_brake(void) {
    MOTOR_DIR_PORT |= (1 << MOTOR_L_IN1_PIN) | (1 << MOTOR_L_IN2_PIN);
}

static void set_right_forward(void) {
    MOTOR_DIR_PORT |=  (1 << MOTOR_R_IN1_PIN);
    MOTOR_DIR_PORT &= ~(1 << MOTOR_R_IN2_PIN);
}
static void set_right_reverse(void) {
    MOTOR_DIR_PORT &= ~(1 << MOTOR_R_IN1_PIN);
    MOTOR_DIR_PORT |=  (1 << MOTOR_R_IN2_PIN);
}
static void set_right_brake(void) {
    MOTOR_DIR_PORT |= (1 << MOTOR_R_IN1_PIN) | (1 << MOTOR_R_IN2_PIN);
}

/* ──────────────────────────────────────────
 *  Motor_Init
 * ────────────────────────────────────────── */
void Motor_Init(void) {

    /* 1. PWM pins as OUTPUT */
    MOTOR_L_PWM_DDR |= (1 << MOTOR_L_PWM_PIN);   /* PD5 OC0B */
    MOTOR_R_PWM_DDR |= (1 << MOTOR_R_PWM_PIN);   /* PD6 OC0A */

    /* 2. Direction pins as OUTPUT */
    MOTOR_DIR_DDR |= (1 << MOTOR_L_IN1_PIN) | (1 << MOTOR_L_IN2_PIN)
                   | (1 << MOTOR_R_IN1_PIN) | (1 << MOTOR_R_IN2_PIN);

    /* 3. STBY pin as OUTPUT, drive HIGH to enable the driver */
    MOTOR_STBY_DDR  |=  (1 << MOTOR_STBY_PIN);
    MOTOR_STBY_PORT |=  (1 << MOTOR_STBY_PIN);   /* HIGH = active */

    /* 4. Configure Timer0 for Fast PWM
     *
     *    TCCR0A bits:
     *      COM0A1=1, COM0A0=0 → OC0A non-inverting (right motor)
     *      COM0B1=1, COM0B0=0 → OC0B non-inverting (left  motor)
     *      WGM01=1,  WGM00=1  → Fast PWM mode (WGM=011)
     *
     *    TCCR0B bits:
     *      CS01=1 → prescaler /8
     */
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01);

    /* 5. Start with both motors stopped */
    Motor_Stop();
}

/* ──────────────────────────────────────────
 *  Motor_SetSpeed
 *  left / right : -255 to +255
 *  0 = brake
 * ────────────────────────────────────────── */
void Motor_SetSpeed(int left, int right) {

    /* ── Left motor ── */
    if (left > 0) {
        set_left_forward();
        if (left > 255) left = 255;
        OCR0B = (uint8_t)left;
    } else if (left < 0) {
        set_left_reverse();
        if (left < -255) left = -255;
        OCR0B = (uint8_t)(-left);
    } else {
        set_left_brake();
        OCR0B = 0;
    }

    /* ── Right motor ── */
    if (right > 0) {
        set_right_forward();
        if (right > 255) right = 255;
        OCR0A = (uint8_t)right;
    } else if (right < 0) {
        set_right_reverse();
        if (right < -255) right = -255;
        OCR0A = (uint8_t)(-right);
    } else {
        set_right_brake();
        OCR0A = 0;
    }
}

/* ──────────────────────────────────────────
 *  Motor_Stop
 * ────────────────────────────────────────── */
void Motor_Stop(void) {
    set_left_brake();
    set_right_brake();
    OCR0A = 0;
    OCR0B = 0;
}

/* ──────────────────────────────────────────
 *  Convenience wrappers for FSM
 * ────────────────────────────────────────── */
void Motor_Forward(void) {
    Motor_SetSpeed(MOTOR_BASE_SPEED, MOTOR_BASE_SPEED);
}

/* Point turn left: left wheel back, right wheel forward */
void Motor_TurnLeft(void) {
    Motor_SetSpeed(-MOTOR_TURN_SPEED, MOTOR_TURN_SPEED);
}

/* Point turn right: left wheel forward, right wheel back */
void Motor_TurnRight(void) {
    Motor_SetSpeed(MOTOR_TURN_SPEED, -MOTOR_TURN_SPEED);
}

void Motor_SlowDown(void) {
    Motor_SetSpeed(MOTOR_SLOW_SPEED, MOTOR_SLOW_SPEED);
}