#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include <stdint.h>
#include "../config/pins.h"

/* ══════════════════════════════════════════════
 *  ir_sensor.h — LM393 Digital IR Obstacle Sensor
 *  Team 11 Wall Following Robot
 *
 *  This is NOT used for wall following distance.
 *  It is used ONLY as an emergency front stop:
 *  "Is something dangerously close to the front?"
 *
 *  Output is binary: 1 = obstacle detected, 0 = clear.
 *  Tune the potentiometer on the module to ~8cm range.
 *
 *  The sensor output is ACTIVE LOW:
 *  Pin reads LOW  (0) → obstacle detected
 *  Pin reads HIGH (1) → no obstacle
 * ══════════════════════════════════════════════ */

/* Call once in main() */
void    IR_Init(void);

/* Returns 1 if obstacle detected within set range, 0 if clear */
uint8_t IR_ObstacleDetected(void);

#endif /* IR_SENSOR_H */