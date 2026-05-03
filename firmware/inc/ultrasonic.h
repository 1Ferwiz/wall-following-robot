#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>

/*
 * Ultrasonic_Init — configure Timer1 ICP and trigger pin
 * Call once at startup
 */
void Ultrasonic_Init(void);

/*
 * Ultrasonic_Trigger — fire a 10µs trigger pulse
 * Call every 20ms from main loop (never faster — sensor needs recovery time)
 */
void Ultrasonic_Trigger(void);

/*
 * Ultrasonic_GetDistance — returns last measured distance in mm
 * Non-blocking — returns cached result from last completed echo
 * Returns 0 if no valid measurement yet or object out of range (>2000mm)
 */
uint16_t Ultrasonic_GetDistance(void);

#endif /* ULTRASONIC_H */