#include "fsm.h"
#include <stdint.h>
#include <stdbool.h>

// simple software timer
volatile uint32_t millis = 0;

void SysTick_Handler(void)
{
    millis++;
}

int main(void)
{
    // Init system (timers later)
    FSM_Init();

    uint32_t lastRun = 0;

    while (1)
    {
        // run every 20 ms (like your design)
        if ((millis - lastRun) >= 20)
        {
            lastRun = millis;
            FSM_Update();
        }
    }

    return 0;
}