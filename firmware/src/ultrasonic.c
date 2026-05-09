#include "../inc/ultrasonic.h"

#include <avr/io.h>
#include <util/delay.h>

#define US_TIMEOUT_TICKS 12000u  /* 6 ms at Timer1 /8, about 1 m range */

static uint16_t dist_mm[3] = { US_NO_ECHO, US_NO_ECHO, US_NO_ECHO };
static uint8_t active_sensor = 0;

static uint16_t measure_echo(volatile uint8_t *trig_port,
                             uint8_t trig_pin,
                             volatile uint8_t *echo_pinreg,
                             uint8_t echo_pin) {
    *trig_port &= (uint8_t)~(1 << trig_pin);
    _delay_us(2);
    *trig_port |= (1 << trig_pin);
    _delay_us(10);
    *trig_port &= (uint8_t)~(1 << trig_pin);

    TCNT1 = 0;
    while (!(*echo_pinreg & (1 << echo_pin))) {
        if (TCNT1 >= US_TIMEOUT_TICKS) {
            return US_NO_ECHO;
        }
    }

    TCNT1 = 0;
    while (*echo_pinreg & (1 << echo_pin)) {
        if (TCNT1 >= US_TIMEOUT_TICKS) {
            return US_NO_ECHO;
        }
    }

    return (uint16_t)(((uint32_t)TCNT1 * 343UL) / 4000UL);
}

void Ultrasonic_Init(void) {
    US_FRONT_TRIG_DDR  |=  (1 << US_FRONT_TRIG_PIN);
    US_FRONT_TRIG_PORT &= (uint8_t)~(1 << US_FRONT_TRIG_PIN);
    US_FRONT_ECHO_DDR  &= (uint8_t)~(1 << US_FRONT_ECHO_PIN);

    US_LEFT_TRIG_DDR   |=  (1 << US_LEFT_TRIG_PIN);
    US_LEFT_TRIG_PORT  &= (uint8_t)~(1 << US_LEFT_TRIG_PIN);
    US_LEFT_ECHO_DDR   &= (uint8_t)~(1 << US_LEFT_ECHO_PIN);

    US_RIGHT_TRIG_DDR  |=  (1 << US_RIGHT_TRIG_PIN);
    US_RIGHT_TRIG_PORT &= (uint8_t)~(1 << US_RIGHT_TRIG_PIN);
    US_RIGHT_ECHO_DDR  &= (uint8_t)~(1 << US_RIGHT_ECHO_PIN);

    TCCR1A = 0x00;
    TCCR1B = (1 << CS11);
    TCNT1 = 0;
}

void Ultrasonic_Update(void) {
    switch (active_sensor) {
        case US_FRONT:
            dist_mm[US_FRONT] = measure_echo(&US_FRONT_TRIG_PORT,
                                             US_FRONT_TRIG_PIN,
                                             &PINB,
                                             US_FRONT_ECHO_PIN);
            break;

        case US_LEFT:
            dist_mm[US_LEFT] = measure_echo(&US_LEFT_TRIG_PORT,
                                            US_LEFT_TRIG_PIN,
                                            &US_LEFT_ECHO_PINREG,
                                            US_LEFT_ECHO_PIN);
            break;

        case US_RIGHT:
            dist_mm[US_RIGHT] = measure_echo(&US_RIGHT_TRIG_PORT,
                                             US_RIGHT_TRIG_PIN,
                                             &US_RIGHT_ECHO_PINREG,
                                             US_RIGHT_ECHO_PIN);
            break;

        default:
            active_sensor = US_FRONT;
            return;
    }

    active_sensor = (uint8_t)((active_sensor + 1u) % 3u);
}

uint16_t Ultrasonic_GetDistance(US_Sensor_t sensor) {
    if (sensor > US_RIGHT) {
        return US_NO_ECHO;
    }

    return dist_mm[sensor];
}
