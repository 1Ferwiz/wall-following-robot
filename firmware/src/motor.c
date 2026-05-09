#include "../inc/motor.h"

#include <avr/io.h>
#include <stdint.h>

static uint8_t clamp_speed(int speed) {
    if (speed < 0) {
        speed = -speed;
    }
    if (speed > 255) {
        speed = 255;
    }
    return (uint8_t)speed;
}

void Motor_Init(void) {
    MOTOR_L_PWM_DDR |= (1 << MOTOR_L_PWM_PIN);
    MOTOR_R_PWM_DDR |= (1 << MOTOR_R_PWM_PIN);

    MOTOR_DIR_DDR |= (1 << MOTOR_L_DIR_PIN) | (1 << MOTOR_R_DIR_PIN);
    MOTOR_DIR_PORT &= (uint8_t)~((1 << MOTOR_L_DIR_PIN) | (1 << MOTOR_R_DIR_PIN));

    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01);

    Motor_Stop();
}

static void set_left_speed(int speed) {
    uint8_t pwm = clamp_speed(speed);

    if (speed > 0) {
        MOTOR_DIR_PORT &= (uint8_t)~(1 << MOTOR_L_DIR_PIN);
        OCR0B = pwm;
    } else if (speed < 0) {
        MOTOR_DIR_PORT |= (1 << MOTOR_L_DIR_PIN);
        OCR0B = (uint8_t)(255u - pwm);
    } else {
        MOTOR_DIR_PORT &= (uint8_t)~(1 << MOTOR_L_DIR_PIN);
        OCR0B = 0;
    }
}

static void set_right_speed(int speed) {
    uint8_t pwm = clamp_speed(speed);

    if (speed > 0) {
        MOTOR_DIR_PORT &= (uint8_t)~(1 << MOTOR_R_DIR_PIN);
        OCR0A = pwm;
    } else if (speed < 0) {
        MOTOR_DIR_PORT |= (1 << MOTOR_R_DIR_PIN);
        OCR0A = (uint8_t)(255u - pwm);
    } else {
        MOTOR_DIR_PORT &= (uint8_t)~(1 << MOTOR_R_DIR_PIN);
        OCR0A = 0;
    }
}

void Motor_SetSpeed(int left, int right) {
    set_left_speed(left);
    set_right_speed(right);
}

void Motor_Stop(void) {
    Motor_SetSpeed(0, 0);
}

void Motor_Forward(void) {
    Motor_SetSpeed(MOTOR_BASE_SPEED, MOTOR_BASE_SPEED);
}

void Motor_TurnLeft(void) {
    Motor_SetSpeed(-MOTOR_TURN_SPEED, MOTOR_TURN_SPEED);
}

void Motor_TurnRight(void) {
    Motor_SetSpeed(MOTOR_TURN_SPEED, -MOTOR_TURN_SPEED);
}

void Motor_SlowDown(void) {
    Motor_SetSpeed(MOTOR_SLOW_SPEED, MOTOR_SLOW_SPEED);
}
