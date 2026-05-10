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

static void set_motor_direction(uint8_t in1_pin, uint8_t in2_pin, int speed) {
    uint8_t mask = (uint8_t)((1 << in1_pin) | (1 << in2_pin));

    if (speed > 0) {
        MOTOR_IN_PORT = (uint8_t)((MOTOR_IN_PORT & (uint8_t)~mask) | (1 << in1_pin));
    } else if (speed < 0) {
        MOTOR_IN_PORT = (uint8_t)((MOTOR_IN_PORT & (uint8_t)~mask) | (1 << in2_pin));
    } else {
        MOTOR_IN_PORT &= (uint8_t)~mask;
    }
}

void Motor_Init(void) {
    MOTOR_L_EN_DDR |= (1 << MOTOR_L_EN_PIN);
    MOTOR_R_EN_DDR |= (1 << MOTOR_R_EN_PIN);

    MOTOR_IN_DDR |= (1 << MOTOR_L_IN1_PIN) | (1 << MOTOR_L_IN2_PIN) |
                    (1 << MOTOR_R_IN1_PIN) | (1 << MOTOR_R_IN2_PIN);
    MOTOR_IN_PORT &= (uint8_t)~((1 << MOTOR_L_IN1_PIN) | (1 << MOTOR_L_IN2_PIN) |
                                (1 << MOTOR_R_IN1_PIN) | (1 << MOTOR_R_IN2_PIN));

    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01);

    Motor_Stop();
}

static void set_left_speed(int speed) {
    uint8_t pwm = clamp_speed(speed);

    set_motor_direction(MOTOR_L_IN1_PIN, MOTOR_L_IN2_PIN, speed);
    OCR0B = pwm;
}

static void set_right_speed(int speed) {
    uint8_t pwm = clamp_speed(speed);

    set_motor_direction(MOTOR_R_IN1_PIN, MOTOR_R_IN2_PIN, speed);
    OCR0A = pwm;
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
