#include "../inc/ultrasonic.h"
#include <avr/io.h>
#include <util/delay.h>    /* _delay_us() — only used for 10µs TRIG pulse */

/* ══════════════════════════════════════════════
 *  ultrasonic.c — 3× HC-SR04 driver
 *
 *  Strategy: round-robin, one sensor per Update() call.
 *  Each measurement blocks for at most ~3ms
 *  (echo pulse length at 50cm, speed of sound 343m/s).
 *  At 60cm wall: 2×600mm / 343000mm/s ≈ 3.5ms max block.
 *  Safe to call from main loop scheduler every 20ms.
 *
 *  Timing method:
 *    Front  → Timer1 counter (16-bit, 1µs ticks @ 16MHz prescale 16)
 *    Left   → software loop counter (side sensors, ~8-bit precision OK)
 *    Right  → software loop counter
 *
 *  Speed of sound constant:
 *    343 m/s = 0.343 mm/µs
 *    distance_mm = pulse_µs * 0.343 / 2
 *               = pulse_µs * 343 / 2000
 * ══════════════════════════════════════════════ */

/* ──────────────────────────────────────────
 *  Private state
 * ────────────────────────────────────────── */
static uint16_t dist_mm[3] = { US_NO_ECHO, US_NO_ECHO, US_NO_ECHO };
static uint8_t  active_sensor = 0;   /* cycles 0 → 1 → 2 → 0 ... */

/* ──────────────────────────────────────────
 *  Private helpers — one per sensor
 *  Each triggers its sensor and measures echo
 *  Returns distance in mm, or US_NO_ECHO
 * ────────────────────────────────────────── */

/* FRONT sensor — uses Timer1 counter for accurate timing */
static uint16_t measure_front(void) {

    /* --- TRIG: 10µs HIGH pulse --- */
    US_FRONT_TRIG_PORT |=  (1 << US_FRONT_TRIG_PIN);
    _delay_us(10);
    US_FRONT_TRIG_PORT &= ~(1 << US_FRONT_TRIG_PIN);

    /* --- Wait for echo to go HIGH (with timeout) ---
     * Timeout ~ 600 iterations ≈ enough for 38ms no-echo signal  */
    uint16_t timeout = 0;
    while (!(PINB & (1 << US_FRONT_ECHO_PIN))) {
        if (++timeout > 30000) return US_NO_ECHO;
    }

    /* --- Reset Timer1 counter and count while echo is HIGH --- */
    TCNT1 = 0;
    while (PINB & (1 << US_FRONT_ECHO_PIN)) {
        if (TCNT1 > 30000) return US_NO_ECHO;  /* ~30ms timeout */
    }

    /* TCNT1 counts at F_CPU/8 = 2MHz → 0.5µs per tick
     * pulse_µs = TCNT1 / 2
     * dist_mm  = pulse_µs * 343 / 2000
     *          = TCNT1 * 343 / 4000                              */
    uint32_t ticks = TCNT1;
    uint16_t mm = (uint16_t)((ticks * 343UL) / 4000UL);
    return mm;
}

/* LEFT sensor — software loop timing (Port C) */
static uint16_t measure_left(void) {

    /* --- TRIG: 10µs HIGH pulse --- */
    US_LEFT_TRIG_PORT |=  (1 << US_LEFT_TRIG_PIN);
    _delay_us(10);
    US_LEFT_TRIG_PORT &= ~(1 << US_LEFT_TRIG_PIN);

    /* --- Wait for echo HIGH --- */
    uint16_t timeout = 0;
    while (!(US_LEFT_ECHO_PINREG & (1 << US_LEFT_ECHO_PIN))) {
        if (++timeout > 30000) return US_NO_ECHO;
    }

    /* --- Count loop iterations while echo stays HIGH ---
     * Each loop iteration ≈ 4 clock cycles @ 16MHz = 0.25µs
     * pulse_µs ≈ count * 0.25
     * dist_mm  = count * 0.25 * 343 / 2000
     *          = count * 343 / 8000
     * Note: less precise than Timer1 but ±5mm is fine for side walls */
    uint16_t count = 0;
    while (US_LEFT_ECHO_PINREG & (1 << US_LEFT_ECHO_PIN)) {
        if (++count > 60000) return US_NO_ECHO;
    }

    uint16_t mm = (uint16_t)((uint32_t)count * 343UL / 8000UL);
    return mm;
}

/* RIGHT sensor — software loop timing (Port D TRIG, Port B ECHO) */
static uint16_t measure_right(void) {

    /* --- TRIG: 10µs HIGH pulse --- */
    US_RIGHT_TRIG_PORT |=  (1 << US_RIGHT_TRIG_PIN);
    _delay_us(10);
    US_RIGHT_TRIG_PORT &= ~(1 << US_RIGHT_TRIG_PIN);

    /* --- Wait for echo HIGH --- */
    uint16_t timeout = 0;
    while (!(US_RIGHT_ECHO_PINREG & (1 << US_RIGHT_ECHO_PIN))) {
        if (++timeout > 30000) return US_NO_ECHO;
    }

    /* --- Count while echo HIGH (same method as LEFT) --- */
    uint16_t count = 0;
    while (US_RIGHT_ECHO_PINREG & (1 << US_RIGHT_ECHO_PIN)) {
        if (++count > 60000) return US_NO_ECHO;
    }

    uint16_t mm = (uint16_t)((uint32_t)count * 343UL / 8000UL);
    return mm;
}

/* ──────────────────────────────────────────
 *  Ultrasonic_Init
 * ────────────────────────────────────────── */
void Ultrasonic_Init(void) {

    /* FRONT TRIG → OUTPUT, ECHO → INPUT */
    US_FRONT_TRIG_DDR  |=  (1 << US_FRONT_TRIG_PIN);
    US_FRONT_TRIG_PORT &= ~(1 << US_FRONT_TRIG_PIN);   /* start LOW  */
    US_FRONT_ECHO_DDR  &= ~(1 << US_FRONT_ECHO_PIN);   /* input      */

    /* LEFT TRIG → OUTPUT, ECHO → INPUT */
    US_LEFT_TRIG_DDR   |=  (1 << US_LEFT_TRIG_PIN);
    US_LEFT_TRIG_PORT  &= ~(1 << US_LEFT_TRIG_PIN);
    US_LEFT_ECHO_DDR   &= ~(1 << US_LEFT_ECHO_PIN);

    /* RIGHT TRIG → OUTPUT, ECHO → INPUT */
    US_RIGHT_TRIG_DDR  |=  (1 << US_RIGHT_TRIG_PIN);
    US_RIGHT_TRIG_PORT &= ~(1 << US_RIGHT_TRIG_PIN);
    US_RIGHT_ECHO_DDR  &= ~(1 << US_RIGHT_ECHO_PIN);

    /* Configure Timer1 for free-running mode, prescaler /8
     * → 2 MHz clock → 0.5µs per tick, overflows after 32.7ms
     * This is used only to time the FRONT echo pulse.           */
    TCCR1A = 0x00;                  /* normal mode               */
    TCCR1B = (1 << CS11);           /* prescaler /8              */
    TCNT1  = 0;
}

/* ──────────────────────────────────────────
 *  Ultrasonic_Update
 *  Call every ~20ms from your main loop.
 *  Measures ONE sensor per call and advances
 *  to the next sensor for the next call.
 * ────────────────────────────────────────── */
void Ultrasonic_Update(void) {
    switch (active_sensor) {
        case US_FRONT:
            dist_mm[US_FRONT] = measure_front();
            break;
        case US_LEFT:
            dist_mm[US_LEFT]  = measure_left();
            break;
        case US_RIGHT:
            dist_mm[US_RIGHT] = measure_right();
            break;
    }
    /* Advance to next sensor (0→1→2→0) */
    active_sensor = (active_sensor + 1) % 3;
}

/* ──────────────────────────────────────────
 *  Ultrasonic_GetDistance
 *  Returns last measured distance in mm.
 *  Non-blocking — just reads stored result.
 *
 *  Usage:
 *    uint16_t left  = Ultrasonic_GetDistance(US_LEFT);
 *    uint16_t front = Ultrasonic_GetDistance(US_FRONT);
 * ────────────────────────────────────────── */
uint16_t Ultrasonic_GetDistance(US_Sensor_t sensor) {
    return dist_mm[sensor];
}