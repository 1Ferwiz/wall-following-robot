#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <avr/io.h>
#include "pins.h"          // all pin definitions live here

/* ──────────────────────────────────────────
 *  Motor & Wheel Constants
 *  GA25-370 — 12V 250RPM confirmed from motor label
 * ────────────────────────────────────────── */
#define ENCODER_PPR             11      // pulses per motor shaft revolution
#define ENCODER_GEAR_RATIO      30      // 7500 base RPM / 250 output RPM = 30
#define TICKS_PER_REV           (ENCODER_PPR * ENCODER_GEAR_RATIO)  // = 330

#define WHEEL_DIAMETER_CM       6.5     // measure your real wheel and update this!
#define WHEEL_CIRCUM_CM         (3.14159265f * WHEEL_DIAMETER_CM)

/* ──────────────────────────────────────────
 *  90° Turn Tick Target
 *  For a differential-drive robot turning on the spot:
 *  Arc each wheel travels = (π × wheelbase) / 4
 *  Example wheelbase = 14 cm → arc = 10.99 cm
 *  Ticks for 90° = arc / cm_per_tick
 *  UPDATE WHEELBASE_CM to your actual chassis measurement
 * ────────────────────────────────────────── */
#define WHEELBASE_CM            14.0f   // distance between left & right wheels
#define TURN_90_ARC_CM          (3.14159265f * WHEELBASE_CM / 4.0f)
#define CM_PER_TICK             (WHEEL_CIRCUM_CM / TICKS_PER_REV)
#define TICKS_FOR_90_TURN       (uint32_t)(TURN_90_ARC_CM / CM_PER_TICK)

/* ──────────────────────────────────────────
 *  Public API
 * ────────────────────────────────────────── */

// Call once before main loop — sets up pins + external interrupts
void     Encoder_Init(void);

// Read current tick counts (safe to call from main loop anytime)
int32_t  Encoder_GetLeftTicks(void);
int32_t  Encoder_GetRightTicks(void);

// Zero the counters before a controlled move
void     Encoder_ResetLeft(void);
void     Encoder_ResetRight(void);
void     Encoder_ResetAll(void);

// Convert ticks → centimeters
float    Encoder_TicksToCm(int32_t ticks);

// Convert a target distance in cm → how many ticks to wait for
uint32_t Encoder_CmToTicks(float cm);

// Returns 1 if both wheels have reached their tick targets (used for turns)
uint8_t  Encoder_TurnComplete(uint32_t target_ticks);

#endif /* ENCODER_H */