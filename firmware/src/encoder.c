#include "../inc/encoder.h"
#include <avr/interrupt.h>   // ISR()
#include <util/atomic.h>     // ATOMIC_BLOCK

/* ──────────────────────────────────────────
 *  Private counters
 *  static  → invisible outside this file
 *  volatile → must re-read from RAM every time;
 *             value can change inside an ISR
 * ────────────────────────────────────────── */
static volatile int32_t enc_left  = 0;
static volatile int32_t enc_right = 0;

/* ──────────────────────────────────────────
 *  Encoder_Init
 * ────────────────────────────────────────── */
void Encoder_Init(void) {

    /* GA25-370 encoder inputs: A on INT0/INT1, B kept available. */
    ENC_A_DDR &= ~((1 << ENC_L_A_PIN) | (1 << ENC_R_A_PIN));
    ENC_A_PORT |= (1 << ENC_L_A_PIN) | (1 << ENC_R_A_PIN);

    ENC_B_DDR &= ~((1 << ENC_L_B_PIN) | (1 << ENC_R_B_PIN));
    ENC_B_PORT |= (1 << ENC_L_B_PIN) | (1 << ENC_R_B_PIN);

    Encoder_Reset();

    EICRA = (EICRA & ~((1 << ISC01) | (1 << ISC00) |
                       (1 << ISC11) | (1 << ISC10))) |
            (1 << ISC01) | (1 << ISC00) |
            (1 << ISC11) | (1 << ISC10);
    EIMSK |= (1 << INT0) | (1 << INT1);
}

/* ──────────────────────────────────────────
 *  ISRs
 *  Hardware calls these automatically on each
 *  rising edge. Keep them as short as possible.
 * ────────────────────────────────────────── */
ISR(INT0_vect) {
    enc_left++;
}

ISR(INT1_vect) {
    enc_right++;
}

/* ──────────────────────────────────────────
 *  Agreed Team Interface
 * ────────────────────────────────────────── */

int32_t Encoder_GetLeft(void) {
    int32_t val;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { val = enc_left; }
    return val;
}

int32_t Encoder_GetRight(void) {
    int32_t val;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { val = enc_right; }
    return val;
}

// Resets both counters — call before every controlled move
void Encoder_Reset(void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        enc_left  = 0;
        enc_right = 0;
    }
}

/* ──────────────────────────────────────────
 *  Extended Helpers
 * ────────────────────────────────────────── */

// Ticks → centimeters
float Encoder_TicksToCm(int32_t ticks) {
    return ((float)ticks / (float)TICKS_PER_REV) * WHEEL_CIRCUM_CM;
}

// Centimeters → ticks  (use this to set move targets)
uint32_t Encoder_CmToTicks(float cm) {
    return (uint32_t)((cm / WHEEL_CIRCUM_CM) * (float)TICKS_PER_REV);
}

// Returns 1 when BOTH wheels have reached target_ticks
// FSM calls this every loop iteration during a turn:
//
//   Encoder_Reset();
//   Motor_SetSpeed(-150, 150);          // spin in place
//   while (!Encoder_TurnComplete(TICKS_FOR_90_TURN));
//   Motor_Stop();
//
uint8_t Encoder_TurnComplete(uint32_t target_ticks) {
    int32_t l, r;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        l = enc_left;
        r = enc_right;
    }
    // Use absolute values — one wheel spins backward during a turn
    uint32_t l_abs = (l < 0) ? (uint32_t)(-l) : (uint32_t)(l);
    uint32_t r_abs = (r < 0) ? (uint32_t)(-r) : (uint32_t)(r);

    return (l_abs >= target_ticks && r_abs >= target_ticks) ? 1 : 0;
}
