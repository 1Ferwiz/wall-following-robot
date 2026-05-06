#include "../inc/uart.h"
#include "../config/pins.h"
#include <avr/io.h>
#include <stdio.h>

/* ══════════════════════════════════════════════
 *  uart.c — USART0 register-level driver
 *
 *  Baud rate register:
 *   UBRR = (F_CPU / (16 * BAUD)) - 1
 *        = (16000000 / (16 * 9600)) - 1
 *        = 103
 * ══════════════════════════════════════════════ */

#define UBRR_VAL    ((F_CPU / (16UL * UART_BAUD)) - 1)

void UART_Init(void) {
    /* Baud rate — high byte then low byte */
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)(UBRR_VAL);

    /* Enable transmitter and receiver */
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    /* Frame format: 8 data bits, no parity, 1 stop bit (8N1)
     * UCSZ01=1, UCSZ00=1 → 8 bits
     * USBS0=0             → 1 stop bit
     * UPM01=0, UPM00=0   → no parity                        */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

/* ──────────────────────────────────────────
 *  UART_SendChar
 *  Waits until transmit buffer is empty, then sends.
 *  Blocking but only for ~1ms at 9600 baud.
 * ────────────────────────────────────────── */
void UART_SendChar(char c) {
    /* UDRE0 flag = 1 when UDR0 register is empty and ready */
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = (uint8_t)c;
}

/* ──────────────────────────────────────────
 *  UART_SendString
 * ────────────────────────────────────────── */
void UART_SendString(const char *s) {
    while (*s) {
        UART_SendChar(*s++);
    }
}

/* ──────────────────────────────────────────
 *  UART_Available
 *  Returns 1 if a byte has been received and
 *  is waiting in the receive buffer.
 * ────────────────────────────────────────── */
uint8_t UART_Available(void) {
    return (UCSR0A & (1 << RXC0)) ? 1 : 0;
}

/* ──────────────────────────────────────────
 *  UART_ReadChar
 *  Waits for a byte and returns it.
 * ────────────────────────────────────────── */
char UART_ReadChar(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return (char)UDR0;
}

/* ──────────────────────────────────────────
 *  UART_SendUInt
 *  Sends a decimal integer as ASCII digits.
 *  e.g. 42 → sends '4' then '2'
 * ────────────────────────────────────────── */
void UART_SendUInt(uint16_t val) {
    char buf[6];      /* max 5 digits for uint16 + null */
    uint8_t i = 0;

    if (val == 0) {
        UART_SendChar('0');
        return;
    }
    /* Build digits in reverse */
    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    /* Send in correct order */
    while (i > 0) {
        UART_SendChar(buf[--i]);
    }
}

void UART_SendNewline(void) {
    UART_SendChar('\r');
    UART_SendChar('\n');
}

/* ──────────────────────────────────────────
 *  UART_SendTurnReport
 *  Sends the competition-required format:
 *
 *   Turns: 5
 *   Sequence: L, R, R, L, L
 *
 *  sequence[] holds 'L' or 'R' per turn index.
 *  e.g. sequence = {'L','R','R','L','L'}, count = 5
 * ────────────────────────────────────────── */
void UART_SendTurnReport(uint8_t count, const char *sequence) {
    UART_SendString("Turns: ");
    UART_SendUInt(count);
    UART_SendNewline();

    UART_SendString("Sequence: ");
    for (uint8_t i = 0; i < count; i++) {
        UART_SendChar(sequence[i]);
        if (i < count - 1) {
            UART_SendString(", ");
        }
    }
    UART_SendNewline();
}