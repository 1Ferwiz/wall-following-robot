#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

/*
 * pins.h - Wall Following Robot - Team 11
 * Central pin map for every firmware driver.
 */

/* HC-SR04 ultrasonic sensors */
#define US_FRONT_TRIG_DDR       DDRD
#define US_FRONT_TRIG_PORT      PORTD
#define US_FRONT_TRIG_PIN       PD4        /* D4 */

#define US_FRONT_ECHO_DDR       DDRB
#define US_FRONT_ECHO_PORT      PORTB
#define US_FRONT_ECHO_PIN       PB0        /* D8 / ICP1 */

#define US_LEFT_TRIG_DDR        DDRC
#define US_LEFT_TRIG_PORT       PORTC
#define US_LEFT_TRIG_PIN        PC0        /* A0 */

#define US_LEFT_ECHO_DDR        DDRC
#define US_LEFT_ECHO_PORT       PORTC
#define US_LEFT_ECHO_PINREG     PINC
#define US_LEFT_ECHO_PIN        PC1        /* A1 */

#define US_RIGHT_TRIG_DDR       DDRD
#define US_RIGHT_TRIG_PORT      PORTD
#define US_RIGHT_TRIG_PIN       PD7        /* D7 */

#define US_RIGHT_ECHO_DDR       DDRB
#define US_RIGHT_ECHO_PORT      PORTB
#define US_RIGHT_ECHO_PINREG    PINB
#define US_RIGHT_ECHO_PIN       PB3        /* D11 */

/* L298N dual H-bridge motor driver */
#define MOTOR_L_EN_DDR          DDRD
#define MOTOR_L_EN_PORT         PORTD
#define MOTOR_L_EN_PIN          PD5        /* D5 / OC0B -> ENA */

#define MOTOR_R_EN_DDR          DDRD
#define MOTOR_R_EN_PORT         PORTD
#define MOTOR_R_EN_PIN          PD6        /* D6 / OC0A -> ENB */

#define MOTOR_IN_DDR            DDRC
#define MOTOR_IN_PORT           PORTC
#define MOTOR_L_IN1_PIN         PC2        /* A2 -> IN1 */
#define MOTOR_L_IN2_PIN         PC3        /* A3 -> IN2 */
#define MOTOR_R_IN1_PIN         PC4        /* A4 -> IN3 */
#define MOTOR_R_IN2_PIN         PC5        /* A5 -> IN4 */

/* GA25-370 encoder inputs */
#define ENC_A_DDR               DDRD
#define ENC_A_PORT              PORTD
#define ENC_A_PINREG            PIND
#define ENC_L_A_PIN             PD2        /* D2 / INT0 */
#define ENC_R_A_PIN             PD3        /* D3 / INT1 */

#define ENC_B_DDR               DDRB
#define ENC_B_PORT              PORTB
#define ENC_B_PINREG            PINB
#define ENC_L_B_PIN             PB1        /* D9 */
#define ENC_R_B_PIN             PB2        /* D10 */

/* UART / HC-05: PD0=RX, PD1=TX fixed by hardware */
#define UART_BAUD               9600UL

/*
 * Timer assignment:
 * Timer0 - motor PWM (OC0A=D6, OC0B=D5)
 * Timer1 - ultrasonic timing
 * Timer2 - 1 ms system tick
 */

#endif /* PINS_H */
