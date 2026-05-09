#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "../inc/systick.h"

static volatile uint32_t ms_ticks = 0;

ISR(TIMER2_COMPA_vect) {
    ms_ticks++;
}

void SysTick_Init(void) {
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS21) | (1 << CS20);
    OCR2A = 249;
    TIMSK2 = (1 << OCIE2A);
}

uint32_t SysTick_GetMs(void) {
    uint32_t value;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        value = ms_ticks;
    }
    return value;
}
