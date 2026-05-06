#include "../inc/ir_sensor.h"
#include <avr/io.h>

/* ══════════════════════════════════════════════
 *  ir_sensor.c — LM393 Digital IR
 *
 *  The module pulls its OUT pin LOW when an obstacle
 *  is detected. We use an internal pull-up so the pin
 *  rests HIGH when nothing is there.
 *
 *  IR_PIN = PB5 (Arduino D13)
 * ══════════════════════════════════════════════ */

void IR_Init(void) {
    /* Set IR pin as INPUT */
    IR_DDR &= ~(1 << IR_PIN);

    /* Enable internal pull-up — pin rests HIGH (no obstacle) */
    IR_PORT |= (1 << IR_PIN);
}

uint8_t IR_ObstacleDetected(void) {
    /* Sensor pulls pin LOW when obstacle is present
     * We invert: LOW pin → return 1 (obstacle detected)  */
    return (IR_PINREG & (1 << IR_PIN)) ? 0 : 1;
}