#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

/* ══════════════════════════════════════════════
 *  systick.c — 1ms tick using Timer2 CTC
 *
 *  Timer2 CTC calculation:
 *   F_CPU = 16 000 000 Hz
 *   Prescaler = /64  → timer clock = 250 000 Hz
 *   OCR2A = 249      → compare match every 250 ticks
 *   Match frequency  = 250 000 / 250 = 1000 Hz = 1ms ✓
 * ══════════════════════════════════════════════ */

static volatile uint32_t ms_ticks = 0;

ISR(TIMER2_COMPA_vect) {
    ms_ticks++;
}

void SysTick_Init(void) {
    /* CTC mode: WGM22=0, WGM21=1, WGM20=0 */
    TCCR2A = (1 << WGM21);

    /* Prescaler /64: CS22=0, CS21=1, CS20=1 */
    TCCR2B = (1 << CS21) | (1 << CS20);

    /* Match value for 1ms */
    OCR2A = 249;

    /* Enable compare match A interrupt */
    TIMSK2 = (1 << OCIE2A);

    /* Global interrupts must be enabled by caller (sei() in main) */
}

uint32_t SysTick_GetMs(void) {
    uint32_t val;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { val = ms_ticks; }
    return val;
}

void SysTick_DelayMs(uint32_t ms) {
    uint32_t start = SysTick_GetMs();
    while ((SysTick_GetMs() - start) < ms);
}