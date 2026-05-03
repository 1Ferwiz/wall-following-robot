#include "fsm.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <stdint.h>

volatile uint32_t millis = 0;

static void Timer2_InitMillis(void)
{
    /*
     * Timer2 CTC, 1 ms tick:
     * 16 MHz / 64 = 250 kHz, 250 counts = 1 ms, OCR2A = 249.
     */
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22);
    OCR2A = 249;
    TIMSK2 = (1 << OCIE2A);
}

static uint32_t millis_read(void)
{
    uint32_t now;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        now = millis;
    }

    return now;
}

ISR(TIMER2_COMPA_vect)
{
    millis++;
}

int main(void)
{
    Timer2_InitMillis();
    FSM_Init();
    sei();

    uint32_t lastRun = 0;

    while (1)
    {
        uint32_t now = millis_read();

        if ((now - lastRun) >= 20)
        {
            lastRun = now;
            FSM_Update();
        }
    }

    return 0;
}
