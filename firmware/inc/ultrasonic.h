#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>
#include "../config/pins.h"

/* ══════════════════════════════════════════════
 *  ultrasonic.h — 3× HC-SR04 driver
 *  Team 11 Wall Following Robot
 *
 *  FRONT  → turn detection & front wall avoidance
 *  LEFT   → left wall distance  (wall following)
 *  RIGHT  → right wall distance (wall following)
 *
 *  All distances returned in millimetres (mm).
 *  Returns US_NO_ECHO (65535) if nothing detected.
 * ══════════════════════════════════════════════ */

/* Returned when echo times out (nothing in range) */
#define US_NO_ECHO          65535U

/* Distances in mm for FSM decisions */
#define US_WALL_TARGET_MM   150U   /* ideal side wall distance — tune this */
#define US_WALL_TOO_CLOSE   80U    /* steer away if side wall < this       */
#define US_WALL_TOO_FAR     220U   /* steer toward if side wall > this     */
#define US_FRONT_TURN_MM    200U   /* front wall → start turn sequence     */
#define US_FRONT_STOP_MM    60U    /* front wall → emergency stop          */

/* Sensor IDs — pass to Ultrasonic_GetDistance() */
typedef enum {
    US_FRONT = 0,
    US_LEFT  = 1,
    US_RIGHT = 2
} US_Sensor_t;

/* ──────────────────────────────────────────
 *  Agreed Team Interface
 * ────────────────────────────────────────── */

/* Call once in main() before the loop */
void     Ultrasonic_Init(void);

/* Call every ~20ms from main loop.
 * Measures one sensor per call, cycling FRONT→LEFT→RIGHT→FRONT...
 * Blocks for at most ~3ms per call (echo pulse at 45cm wall).     */
void     Ultrasonic_Update(void);

/* Returns last measured distance in mm for the given sensor.
 * Non-blocking — just reads the last stored result.               */
uint16_t Ultrasonic_GetDistance(US_Sensor_t sensor);

#endif /* ULTRASONIC_H */