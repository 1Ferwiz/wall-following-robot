#include "encoder.h"
#include <avr/interrupt.h>  // ISR(), sei()
#include <util/atomic.h>    // ATOMIC_BLOCK

/* ──────────────────────────────────────────
 *  Private counters
 *  volatile — values change inside ISRs,
 *  compiler must never cache them
 * ────────────────────────────────────────── */
static volatile int32_t left_ticks  = 0;
static volatile int32_t right_ticks = 0;

/* ──────────────────────────────────────────
 *  Encoder_Init
 * ────────────────────────────────────────── */
void Encoder_Init(void) {

    /* 1. Set ENC_L_PIN (PD2) and ENC_R_PIN (PD3) as INPUT
     *    Clearing the DDR bit makes it an input               */
    ENC_DDR &= ~(1 << ENC_L_PIN);      // PD2 → INPUT
    ENC_DDR &= ~(1 << ENC_R_PIN);      // PD3 → INPUT

    /* 2. Enable internal pull-ups on both encoder pins
     *    Protects against floating signal if wire is loose    */
    ENC_PORT |= (1 << ENC_L_PIN);      // pull-up on PD2
    ENC_PORT |= (1 << ENC_R_PIN);      // pull-up on PD3

    /* 3. Configure interrupt trigger mode in EICRA
     *
     *    ISCx1=1, ISCx0=1 → trigger on RISING EDGE
     *    (counts when signal goes LOW→HIGH = one new pulse)
     *
     *    INT0 uses bits ISC01, ISC00
     *    INT1 uses bits ISC11, ISC10                          */
    EICRA |= (1 << ISC01) | (1 << ISC00);   // INT0 → rising edge (Left)
    EICRA |= (1 << ISC11) | (1 << ISC10);   // INT1 → rising edge (Right)

    /* 4. Enable INT0 and INT1 in the interrupt mask register  */
    EIMSK |= (1 << INT0);   // Left  encoder interrupt ON
    EIMSK |= (1 << INT1);   // Right encoder interrupt ON

    /* 5. Turn on global interrupts — nothing fires without this */
    sei();
}

/* ──────────────────────────────────────────
 *  ISRs — fire automatically on each pulse
 *  Keep them tiny — just increment the counter
 * ────────────────────────────────────────── */
ISR(INT0_vect) {
    left_ticks++;
}

ISR(INT1_vect) {
    right_ticks++;
}

/* ──────────────────────────────────────────
 *  Getters — ATOMIC_BLOCK prevents a corrupted
 *  read if an ISR fires mid-way through copying
 *  the 4-byte int32
 * ────────────────────────────────────────── */
int32_t Encoder_GetLeftTicks(void) {
    int32_t val;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { val = left_ticks; }
    return val;
}

int32_t Encoder_GetRightTicks(void) {
    int32_t val;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { val = right_ticks; }
    return val;
}

/* ──────────────────────────────────────────
 *  Resets — call before every controlled move
 * ────────────────────────────────────────── */
void Encoder_ResetLeft(void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { left_ticks = 0; }
}

void Encoder_ResetRight(void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { right_ticks = 0; }
}

void Encoder_ResetAll(void) {
    Encoder_ResetLeft();
    Encoder_ResetRight();
}

/* ──────────────────────────────────────────
 *  Unit Conversion Helpers
 * ────────────────────────────────────────── */
float Encoder_TicksToCm(int32_t ticks) {
    return ((float)ticks / (float)TICKS_PER_REV) * WHEEL_CIRCUM_CM;
}

uint32_t Encoder_CmToTicks(float cm) {
    return (uint32_t)((cm / WHEEL_CIRCUM_CM) * (float)TICKS_PER_REV);
}

/* ──────────────────────────────────────────
 *  Encoder_TurnComplete
 *  Returns 1 when BOTH wheels have hit the
 *  tick target — used by FSM during 90° turns
 *
 *  Usage example:
 *    Encoder_ResetAll();
 *    Motor_TurnLeft();
 *    while (!Encoder_TurnComplete(TICKS_FOR_90_TURN));
 *    Motor_Stop();
 * ────────────────────────────────────────── */
uint8_t Encoder_TurnComplete(uint32_t target_ticks) {
    int32_t l, r;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        l = left_ticks;
        r = right_ticks;
    }
    // abs() because one wheel may spin backward during a turn
    uint32_t l_abs = (l < 0) ? (uint32_t)(-l) : (uint32_t)(l);
    uint32_t r_abs = (r < 0) ? (uint32_t)(-r) : (uint32_t)(r);

    return (l_abs >= target_ticks && r_abs >= target_ticks) ? 1 : 0;
}