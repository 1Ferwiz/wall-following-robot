#include <avr/io.h>
#include <avr/interrupt.h>

#include "../config/pins.h"
#include "../inc/systick.h"
#include "../inc/motor.h"
#include "../inc/ultrasonic.h"
#include "../inc/ir_sensor.h"
#include "../inc/encoder.h"
#include "../inc/uart.h"
#include "../inc/fsm.h"

/* ══════════════════════════════════════════════
 *  main.c — Wall Following Robot — Team 11
 *
 *  Loop schedule (approximate, per iteration):
 *   Every  20ms → Ultrasonic_Update() — one sensor per call
 *   Every   1ms → FSM_Update()        — state machine tick
 *
 *  All timing is driven by SysTick_GetMs().
 *  No blocking delays in the main loop.
 * ══════════════════════════════════════════════ */

int main(void) {

    /* ── 1. System tick first — everything else uses it ── */
    SysTick_Init();

    /* ── 2. Enable global interrupts (needed by SysTick & Encoder) ── */
    sei();

    /* ── 3. Peripheral init ── */
    Motor_Init();
    Ultrasonic_Init();
    IR_Init();
    Encoder_Init();    /* pins reserved — no effect with TT motor */
    UART_Init();

    /* ── 4. FSM init ── */
    FSM_Init();

    /* ── 5. Send startup message so you know the robot is alive ── */
    UART_SendString("Team 11 Robot Ready\r\n");

    /* ── 6. Main loop variables ── */
    uint32_t last_us_update = 0;   /* timestamp of last ultrasonic update */
    uint32_t last_fsm_tick  = 0;   /* timestamp of last FSM update        */

    /* ── 7. Main loop ── */
    while (1) {
        uint32_t now = SysTick_GetMs();

        /* Update one ultrasonic sensor every 20ms
         * (cycles FRONT → LEFT → RIGHT → FRONT ...)
         * Each sensor gets a fresh reading every 60ms.         */
        if ((now - last_us_update) >= 20) {
            Ultrasonic_Update();
            last_us_update = now;
        }

        /* Run the FSM every 1ms */
        if ((now - last_fsm_tick) >= 1) {
            FSM_Update();
            last_fsm_tick = now;
        }
    }

    return 0;   /* never reached */
}