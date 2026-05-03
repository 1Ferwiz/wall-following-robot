#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

/* ──────────────────────────────────────────
 *  ADC / IR Sensors  (Port C)
 * ────────────────────────────────────────── */
#define IR_LEFT_ADC_CH      0          /* PC0 – ADC0 */
#define IR_RIGHT_ADC_CH     1          /* PC1 – ADC1 */

/* ──────────────────────────────────────────
 *  Ultrasonic HC-SR04
 * ────────────────────────────────────────── */
#define US_TRIG_DDR         DDRD
#define US_TRIG_PORT        PORTD
#define US_TRIG_PIN         PD4

#define US_ECHO_DDR         DDRB       /* ICP1 = PB0  – Timer1 Input Capture */
#define US_ECHO_PORT        PORTB
#define US_ECHO_PIN         PB0

/* ──────────────────────────────────────────
 *  Motors PWM  (Timer0, Port D)
 * ────────────────────────────────────────── */
#define MOTOR_L_PWM_DDR     DDRD
#define MOTOR_L_PWM_PORT    PORTD
#define MOTOR_L_PWM_PIN     PD5        /* OC0B */

#define MOTOR_R_PWM_DDR     DDRD
#define MOTOR_R_PWM_PORT    PORTD
#define MOTOR_R_PWM_PIN     PD6        /* OC0A */

/* ──────────────────────────────────────────
 *  Motor Direction  (Port C)
 * ────────────────────────────────────────── */
#define MOTOR_DIR_DDR       DDRC
#define MOTOR_DIR_PORT      PORTC

#define MOTOR_L_IN1_PIN     PC2
#define MOTOR_L_IN2_PIN     PC3
#define MOTOR_R_IN1_PIN     PC4
#define MOTOR_R_IN2_PIN     PC5

/* ──────────────────────────────────────────
 *  TB6612FNG Standby  (Port B)
 * ────────────────────────────────────────── */
#define MOTOR_STBY_DDR      DDRB
#define MOTOR_STBY_PORT     PORTB
#define MOTOR_STBY_PIN      PB4        /* HIGH = active */

/* ──────────────────────────────────────────
 *  Encoders  (INT0 = PD2, INT1 = PD3)
 * ────────────────────────────────────────── */
#define ENC_DDR             DDRD
#define ENC_PORT            PORTD

#define ENC_L_PIN           PD2        /* INT0 */
#define ENC_R_PIN           PD3        /* INT1 */

/* ──────────────────────────────────────────
 *  UART / HC-05  (hardware USART0)
 *  PD0 = RX,  PD1 = TX   (fixed by hardware)
 * ────────────────────────────────────────── */
#define UART_BAUD           9600UL

#endif /* PINS_H */