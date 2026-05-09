#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>
#include "../config/pins.h"

#define US_NO_ECHO          65535U

#define US_WALL_TARGET_MM   150U
#define US_WALL_TOO_CLOSE    80U
#define US_WALL_TOO_FAR     220U
#define US_FRONT_TURN_MM    200U
#define US_FRONT_STOP_MM     60U

typedef enum {
    US_FRONT = 0,
    US_LEFT  = 1,
    US_RIGHT = 2
} US_Sensor_t;

void Ultrasonic_Init(void);

/*
 * Measures one HC-SR04 per call, cycling front -> left -> right.
 * Uses Timer1 for echo pulse timing. The only deliberate delay is the
 * required 10 us trigger pulse; an out-of-range echo times out at about 6 ms.
 */
void Ultrasonic_Update(void);

uint16_t Ultrasonic_GetDistance(US_Sensor_t sensor);

#endif /* ULTRASONIC_H */
