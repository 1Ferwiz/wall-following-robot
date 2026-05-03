#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <avr/io.h>
#include "pins.h"

/* ──────────────────────────────────────────
 *  Motor & Wheel Constants
 *  GA25-370 — 12V 250RPM (confirmed from motor label)
 * ────────────────────────────────────────── */
#define ENCODER_PPR          11       // pulses per motor shaft revolution
#define ENCODER_GEAR_RATIO   30       // 7500 base RPM / 250 output RPM = 30
#define TICKS_PER_REV        (ENCODER_PPR * ENCODER_GEAR_RATIO)  // = 330

#define WHEEL_DIAMETER_CM    6.5f     // ← measure your real wheel and update
#define WHEEL_CIRCUM_CM      (3.14159265f * WHEEL_DIAMETER_CM)

/* ──────────────────────────────────────────
 *  90° Turn Tick Target
 *  Each wheel travels an arc = (π × wheelbase) / 4
 *  during a point turn.
 *  UPDATE WHEELBASE_CM to your real chassis measurement.
 * ────────────────────────────────────────── */
#define WHEELBASE_CM         14.0f
#define TURN_90_ARC_CM       (3.14159265f * WHEELBASE_CM / 4.0f)
#define CM_PER_TICK          (WHEEL_CIRCUM_CM / (float)TICKS_PER_REV)
#define TICKS_FOR_90_TURN    ((uint32_t)(TURN_90_ARC_CM / CM_PER_TICK))

/* ──────────────────────────────────────────
 *  Agreed Team Interface  (do not rename)
 * ────────────────────────────────────────── */
void    Encoder_Init(void);
int32_t Encoder_GetLeft(void);
int32_t Encoder_GetRight(void);
void    Encoder_Reset(void);

/* ──────────────────────────────────────────
 *  Extended Helpers  (used internally by FSM)
 * ────────────────────────────────────────── */
float    Encoder_TicksToCm(int32_t ticks);
uint32_t Encoder_CmToTicks(float cm);
uint8_t  Encoder_TurnComplete(uint32_t target_ticks);

#endif /* ENCODER_H */