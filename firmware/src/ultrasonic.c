#include "ultrasonic.h"
#include "pins.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

/*
 * Strategy: Timer1 Input Capture
 * - Rising edge  → record start timestamp (ICR1)
 * - Falling edge → record stop timestamp, compute pulse width
 * - Distance = pulse_width_us * 0.1715  (sound speed at ~25°C)
 *
 * Timer1: normal mode, prescaler 8 → tick = 0.5µs at 16MHz
 * Max echo ~23ms → 46000 ticks → fits in 16-bit (max 65535)
 */

#define US_MAX_DIST_MM   2000u   /* ignore echoes beyond 2m */
#define US_NO_READING       0u

/* shared with ISR — must be volatile */
static volatile uint16_t us_start      = 0;
static volatile uint16_t us_distance   = US_NO_READING;
static volatile uint8_t  us_measuring  = 0;

/*
 * Ultrasonic_Init
 */
void Ultrasonic_Init(void)
{
    /* Trigger pin: output, start LOW */
    US_TRIG_DDR  |=  (1 << US_TRIG_PIN);
    US_TRIG_PORT &= ~(1 << US_TRIG_PIN);

    /* Echo pin: input, no pull-up (driven by sensor) */
    US_ECHO_DDR  &= ~(1 << US_ECHO_PIN);
    US_ECHO_PORT &= ~(1 << US_ECHO_PIN);

    /*
     * Timer1: normal mode (WGM13:0 = 0000)
     * Prescaler 8 → tick = 0.5µs
     * Input Capture: rising edge first (ICES1 = 1)
     * Input Capture Noise Canceler: enabled (ICNC1 = 1)
     */
    TCCR1A = 0x00;
    TCCR1B = (1 << ICNC1)   /* noise canceler */
           | (1 << ICES1)   /* rising edge first */
           | (1 << CS11);   /* prescaler 8 */

    /* Enable Input Capture interrupt */
    TIMSK1 |= (1 << ICIE1);
}

/*
 * Ultrasonic_Trigger
 * Send 10µs HIGH pulse on TRIG pin.
 * Uses a short busy-wait — only 10µs, acceptable at trigger time.
 * Never call from ISR.
 */
void Ultrasonic_Trigger(void)
{
    if (us_measuring) return; /* previous echo not done yet — skip */

    us_measuring = 1;

    US_TRIG_PORT |=  (1 << US_TRIG_PIN);  /* TRIG HIGH */

    /* 10µs busy wait — 160 cycles at 16MHz */
    __builtin_avr_delay_cycles(160);

    US_TRIG_PORT &= ~(1 << US_TRIG_PIN);  /* TRIG LOW */
}

/*
 * Ultrasonic_GetDistance
 * Returns cached distance in mm from last completed measurement.
 */
uint16_t Ultrasonic_GetDistance(void)
{
    uint16_t d;
    /* atomic read — disable interrupts briefly for 2-byte variable */
    uint8_t sreg = SREG;
    __builtin_avr_cli();
    d = us_distance;
    SREG = sreg;
    return d;
}

/*
 * ISR: Timer1 Input Capture
 * Fires on both rising and falling edges (we toggle ICES1 each time)
 */
ISR(TIMER1_CAPT_vect)
{
    if (TCCR1B & (1 << ICES1))
    {
        /* Was rising edge — record start, switch to falling edge */
        us_start  = ICR1;
        TCCR1B   &= ~(1 << ICES1);  /* now capture falling edge */
    }
    else
    {
        /* Was falling edge — compute pulse width */
        uint16_t pulse_ticks = ICR1 - us_start;  /* wraps correctly */

        /*
         * Each tick = 0.5µs
         * Distance = (pulse_us / 2) * speed_of_sound_mm_per_us
         * speed ~ 0.343 mm/µs
         * Distance_mm = pulse_ticks * 0.5 * 0.343 / 2
         *             = pulse_ticks * 0.08575
         *             ≈ pulse_ticks * 11 / 128   (integer approximation)
         */
        uint16_t dist = (uint16_t)((uint32_t)pulse_ticks * 11u / 128u);

        if (dist > 0 && dist <= US_MAX_DIST_MM)
        {
            us_distance = dist;
        }

        /* Reset for next measurement */
        TCCR1B  |= (1 << ICES1);  /* back to rising edge */
        us_measuring = 0;
    }
}