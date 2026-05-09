#include <avr/interrupt.h>
#include <avr/io.h>

#include "../inc/encoder.h"
#include "../inc/fsm.h"
#include "../inc/motor.h"
#include "../inc/systick.h"
#include "../inc/uart.h"
#include "../inc/ultrasonic.h"

int main(void) {
    SysTick_Init();

    Motor_Init();
    Ultrasonic_Init();
    Encoder_Init();
    UART_Init();
    FSM_Init();

    sei();

    UART_SendString("Team 11 Robot Ready\r\n");

    uint32_t last_us_update = 0;
    uint32_t last_fsm_tick = 0;

    while (1) {
        uint32_t now = SysTick_GetMs();

        if ((now - last_us_update) >= 20u) {
            Ultrasonic_Update();
            last_us_update = now;
        }

        if ((now - last_fsm_tick) >= 1u) {
            FSM_Update();
            last_fsm_tick = now;
        }
    }

    return 0;
}
