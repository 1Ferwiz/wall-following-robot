#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include "../config/pins.h"

/*
 * HW-134A / L9110S driver for two GA25-370 DC gear motors.
 * Speed range: -255 full reverse, +255 full forward.
 * Timer0 Fast PWM frequency is about 7.8 kHz with /8 prescaler.
 */

#define MOTOR_BASE_SPEED        160
#define MOTOR_TURN_SPEED        140
#define MOTOR_SLOW_SPEED         80

/*
 * Timed fallback for a 90 degree turn if encoder feedback is not valid.
 * Encoder completion is preferred in the FSM.
 */
#define MOTOR_TURN_DURATION_MS  550U

void Motor_Init(void);
void Motor_SetSpeed(int left, int right);
void Motor_Stop(void);
void Motor_Forward(void);
void Motor_TurnLeft(void);
void Motor_TurnRight(void);
void Motor_SlowDown(void);

#endif /* MOTOR_H */
