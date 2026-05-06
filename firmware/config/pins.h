#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

/* ══════════════════════════════════════════════
 *  pins.h — Wall Following Robot — Team 11
 *  READ THIS BEFORE WRITING ANY DRIVER
 * ══════════════════════════════════════════════ */

/* ──────────────────────────────────────────
 *  Ultrasonic HC-SR04 ×3
 *  FRONT  → turn detection / front wall
 *  LEFT   → left wall distance
 *  RIGHT  → right wall distance
 * ────────────────────────────────────────── */
#define US_FRONT_TRIG_DDR       DDRD
#define US_FRONT_TRIG_PORT      PORTD
#define US_FRONT_TRIG_PIN       PD4

#define US_FRONT_ECHO_DDR       DDRB
#define US_FRONT_ECHO_PORT      PORTB
#define US_FRONT_ECHO_PIN       PB0        /* ICP1 */

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

/* ──────────────────────────────────────────
 *  Digital IR Obstacle Sensor (LM393)
 *  Emergency front stop only — binary output
 *  LOW = obstacle detected
 * ────────────────────────────────────────── */
#define IR_DDR                  DDRB
#define IR_PORT                 PORTB
#define IR_PINREG               PINB
#define IR_PIN                  PB5        /* D13 */

/* ──────────────────────────────────────────
 *  Motors PWM  (Timer0)
 * ────────────────────────────────────────── */
#define MOTOR_L_PWM_DDR         DDRD
#define MOTOR_L_PWM_PORT        PORTD
#define MOTOR_L_PWM_PIN         PD5        /* OC0B — D5 */

#define MOTOR_R_PWM_DDR         DDRD
#define MOTOR_R_PWM_PORT        PORTD
#define MOTOR_R_PWM_PIN         PD6        /* OC0A — D6 */

/* ──────────────────────────────────────────
 *  Motor Direction  (Port C)
 * ────────────────────────────────────────── */
#define MOTOR_DIR_DDR           DDRC
#define MOTOR_DIR_PORT          PORTC

#define MOTOR_L_IN1_PIN         PC2        /* A2 */
#define MOTOR_L_IN2_PIN         PC3        /* A3 */
#define MOTOR_R_IN1_PIN         PC4        /* A4 */
#define MOTOR_R_IN2_PIN         PC5        /* A5 */

/* ──────────────────────────────────────────
 *  TB6612FNG Standby
 * ────────────────────────────────────────── */
#define MOTOR_STBY_DDR          DDRB
#define MOTOR_STBY_PORT         PORTB
#define MOTOR_STBY_PIN          PB4        /* D12 — HIGH = active */

/* ──────────────────────────────────────────
 *  Encoders  (INT0=PD2, INT1=PD3)
 *  ⚠ CURRENTLY UNUSED — TT motor has no encoder.
 *  Pins reserved for GA25-370 upgrade.
 *  Do NOT assign D2/D3 to other functions.
 * ────────────────────────────────────────── */
#define ENC_DDR                 DDRD
#define ENC_PORT                PORTD
#define ENC_L_PIN               PD2        /* INT0 — reserved */
#define ENC_R_PIN               PD3        /* INT1 — reserved */
#define ENC_L_B_DDR             DDRB
#define ENC_L_B_PIN             PB1        /* D9  — reserved  */
#define ENC_R_B_DDR             DDRB
#define ENC_R_B_PIN             PB2        /* D10 — reserved  */

/* ──────────────────────────────────────────
 *  UART / HC-05
 *  PD0=RX, PD1=TX (fixed by hardware)
 *  ⚠ Disconnect HC-05 TX/RX before uploading!
 * ────────────────────────────────────────── */
#define UART_BAUD               9600UL

/* ──────────────────────────────────────────
 *  Timer Assignment  (DO NOT CHANGE)
 *  Timer0 → Motor PWM  (OC0A=D6, OC0B=D5)
 *  Timer1 → Ultrasonic (free-running, front echo)
 *  Timer2 → System tick (1 ms millis)
 * ────────────────────────────────────────── */

#endif /* PINS_H */