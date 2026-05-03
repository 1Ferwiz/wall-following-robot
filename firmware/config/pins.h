#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

/* ──────────────────────────────────────────
 *  ADC / IR Sensors  (Port C)
 *  Arduino: A0, A1
 * ────────────────────────────────────────── */
#define IR_LEFT_ADC_CH      0          /* PC0 – ADC0 – A0 */
#define IR_RIGHT_ADC_CH     1          /* PC1 – ADC1 – A1 */

/* ──────────────────────────────────────────
 *  Ultrasonic HC-SR04
 *  TRIG: Arduino D4
 *  ECHO: Arduino D8  (ICP1 = PB0)
 * ────────────────────────────────────────── */
#define US_TRIG_DDR         DDRD
#define US_TRIG_PORT        PORTD
#define US_TRIG_PIN         PD4        /* D4 */

#define US_ECHO_DDR         DDRB
#define US_ECHO_PORT        PORTB
#define US_ECHO_PIN         PB0        /* D8 – ICP1 – Timer1 Input Capture */

/* ──────────────────────────────────────────
 *  Motors PWM  (Timer0, Port D)
 *  Left PWM:  Arduino D6 (OC0A)
 *  Right PWM: Arduino D5 (OC0B)
 * ────────────────────────────────────────── */
#define MOTOR_L_PWM_DDR     DDRD
#define MOTOR_L_PWM_PORT    PORTD
#define MOTOR_L_PWM_PIN     PD5        /* D5 - OC0B */

#define MOTOR_R_PWM_DDR     DDRD
#define MOTOR_R_PWM_PORT    PORTD
#define MOTOR_R_PWM_PIN     PD6        /* D6 - OC0A */

/* ──────────────────────────────────────────
 *  Motor Direction  (Port C)
 *  Arduino: A2, A3, A4, A5
 * ────────────────────────────────────────── */
#define MOTOR_DIR_DDR       DDRC
#define MOTOR_DIR_PORT      PORTC

#define MOTOR_L_IN1_PIN     PC2        /* D/A2 */
#define MOTOR_L_IN2_PIN     PC3        /* A3 */
#define MOTOR_R_IN1_PIN     PC4        /* A4 */
#define MOTOR_R_IN2_PIN     PC5        /* A5 */

/* ──────────────────────────────────────────
 *  TB6612FNG Standby  (Port B)
 *  Arduino: D12
 * ────────────────────────────────────────── */
#define MOTOR_STBY_DDR      DDRB
#define MOTOR_STBY_PORT     PORTB
#define MOTOR_STBY_PIN      PB4        /* D12 – HIGH = active */

/* ──────────────────────────────────────────
 *  Encoders
 *  Channel A: INT0 = PD2 = D2  (left)
 *             INT1 = PD3 = D3  (right)
 *  Channel B: PB1 = D9  (left)
 *             PB2 = D10 (right)  — connected, reserved for future use
 * ────────────────────────────────────────── */
#define ENC_A_DDR           DDRD
#define ENC_A_PORT          PORTD
#define ENC_L_A_PIN         PD2        /* D2 – INT0 */
#define ENC_R_A_PIN         PD3        /* D3 – INT1 */

#define ENC_B_DDR           DDRB
#define ENC_B_PORT          PORTB
#define ENC_L_B_PIN         PB1        /* D9  – reserved */
#define ENC_R_B_PIN         PB2        /* D10 – reserved */

/* ──────────────────────────────────────────
 *  UART / HC-05  (hardware USART0)
 *  RX: PD0 = D0  ← HC-05 TX
 *  TX: PD1 = D1  → HC-05 RX
 *  ⚠ Disconnect HC-05 before uploading code
 * ────────────────────────────────────────── */
#define UART_DDR            DDRD
#define UART_PORT           PORTD
#define UART_RX_PIN         PD0
#define UART_TX_PIN         PD1
#define UART_BAUD           9600UL

#endif /* PINS_H */
