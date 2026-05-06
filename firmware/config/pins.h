#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

/* ══════════════════════════════════════════════
 *  pins.h — Wall Following Robot — Team 11
 *  READ THIS BEFORE WRITING ANY DRIVER
 *  ALL pin definitions live here.
 *  Never use raw register names in driver code.
 * ══════════════════════════════════════════════ */

/* ──────────────────────────────────────────
 *  Ultrasonic HC-SR04 ×3
 *
 *  FRONT  — turn detection, front wall
 *  LEFT   — left wall distance  (was Sharp IR)
 *  RIGHT  — right wall distance (was Sharp IR)
 *
 *  Note: A0/A1 repurposed from Sharp IR to
 *        left ultrasonic TRIG/ECHO.
 *        Sharp IR sensors are no longer used.
 * ────────────────────────────────────────── */

/* Front */
#define US_FRONT_TRIG_DDR    DDRD
#define US_FRONT_TRIG_PORT   PORTD
#define US_FRONT_TRIG_PIN    PD4

#define US_FRONT_ECHO_DDR    DDRB       /* ICP1 = PB0 — Timer1 Input Capture */
#define US_FRONT_ECHO_PORT   PORTB
#define US_FRONT_ECHO_PIN    PB0

/* Left (A0=TRIG, A1=ECHO — Port C, digital use) */
#define US_LEFT_TRIG_DDR     DDRC
#define US_LEFT_TRIG_PORT    PORTC
#define US_LEFT_TRIG_PIN     PC0        /* Arduino A0 */

#define US_LEFT_ECHO_DDR     DDRC
#define US_LEFT_ECHO_PORT    PORTC
#define US_LEFT_ECHO_PINREG  PINC
#define US_LEFT_ECHO_PIN     PC1        /* Arduino A1 */

/* Right (D7=TRIG, D11=ECHO — Ports D and B) */
#define US_RIGHT_TRIG_DDR    DDRD
#define US_RIGHT_TRIG_PORT   PORTD
#define US_RIGHT_TRIG_PIN    PD7        /* Arduino D7 */

#define US_RIGHT_ECHO_DDR    DDRB
#define US_RIGHT_ECHO_PORT   PORTB
#define US_RIGHT_ECHO_PINREG PINB
#define US_RIGHT_ECHO_PIN    PB3        /* Arduino D11 */

/* ──────────────────────────────────────────
 *  Digital IR Obstacle Sensor
 *  LM393-based, binary output (LOW = obstacle)
 *  Used as emergency front proximity stop only.
 *  Tune potentiometer to trigger at ~8 cm.
 * ────────────────────────────────────────── */
#define IR_DDR               DDRB
#define IR_PORT              PORTB
#define IR_PINREG            PINB
#define IR_PIN               PB5        /* Arduino D13 */

/* ──────────────────────────────────────────
 *  Motors PWM  (Timer0, Port D)
 * ────────────────────────────────────────── */
#define MOTOR_L_PWM_DDR      DDRD
#define MOTOR_L_PWM_PORT     PORTD
#define MOTOR_L_PWM_PIN      PD5        /* OC0B — Arduino D5 */

#define MOTOR_R_PWM_DDR      DDRD
#define MOTOR_R_PWM_PORT     PORTD
#define MOTOR_R_PWM_PIN      PD6        /* OC0A — Arduino D6 */

/* ──────────────────────────────────────────
 *  Motor Direction  (Port C)
 * ────────────────────────────────────────── */
#define MOTOR_DIR_DDR        DDRC
#define MOTOR_DIR_PORT       PORTC

#define MOTOR_L_IN1_PIN      PC2        /* Arduino A2 */
#define MOTOR_L_IN2_PIN      PC3        /* Arduino A3 */
#define MOTOR_R_IN1_PIN      PC4        /* Arduino A4 */
#define MOTOR_R_IN2_PIN      PC5        /* Arduino A5 */

/* ──────────────────────────────────────────
 *  TB6612FNG Standby  (Port B)
 * ────────────────────────────────────────── */
#define MOTOR_STBY_DDR       DDRB
#define MOTOR_STBY_PORT      PORTB
#define MOTOR_STBY_PIN       PB4        /* Arduino D12 — HIGH = active */

/* ──────────────────────────────────────────
 *  Encoders  (INT0 = PD2, INT1 = PD3)
 * ────────────────────────────────────────── */
#define ENC_DDR              DDRD
#define ENC_PORT             PORTD

#define ENC_L_PIN            PD2        /* INT0 — Arduino D2 */
#define ENC_R_PIN            PD3        /* INT1 — Arduino D3 */

/* Encoder Channel B — wired but unused in code yet */
#define ENC_L_B_DDR          DDRB
#define ENC_L_B_PIN          PB1        /* Arduino D9  */
#define ENC_R_B_DDR          DDRB
#define ENC_R_B_PIN          PB2        /* Arduino D10 */

/* ──────────────────────────────────────────
 *  UART / HC-05  (hardware USART0)
 *  PD0 = RX,  PD1 = TX  (fixed by hardware)
 *  ⚠ Disconnect HC-05 TX/RX before uploading!
 * ────────────────────────────────────────── */
#define UART_BAUD            9600UL

/* ──────────────────────────────────────────
 *  Timer Assignment (DO NOT CHANGE)
 *  Timer0 → Motor PWM     (OC0A=D6, OC0B=D5)
 *  Timer1 → Ultrasonic    (ICP1=D8, front only)
 *  Timer2 → System tick   (1ms millis)
 * ────────────────────────────────────────── */

#endif /* PINS_H */