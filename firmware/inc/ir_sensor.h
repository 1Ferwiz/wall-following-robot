#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include <stdint.h>

/*
 * IR_Init — configure ADC for IR sensors
 * Call once at startup before any IR read
 */
void IR_Init(void);

/*
 * IR_GetLeftDistance  — returns distance in mm (100–800mm)
 * IR_GetRightDistance — returns distance in mm (100–800mm)
 *
 * Returns 0 if reading is out of sensor range
 */
uint16_t IR_GetLeftDistance(void);
uint16_t IR_GetRightDistance(void);

#endif /* IR_SENSOR_H */