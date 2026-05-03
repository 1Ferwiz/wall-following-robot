#include "uart.h"
#include "pins.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define UART_RX_BUFFER_SIZE 64u
#define UART_TX_BUFFER_SIZE 64u
#define UART_RX_MASK        (UART_RX_BUFFER_SIZE - 1u)
#define UART_TX_MASK        (UART_TX_BUFFER_SIZE - 1u)

#if ((UART_RX_BUFFER_SIZE & UART_RX_MASK) != 0)
#error "UART_RX_BUFFER_SIZE must be a power of two"
#endif

#if ((UART_TX_BUFFER_SIZE & UART_TX_MASK) != 0)
#error "UART_TX_BUFFER_SIZE must be a power of two"
#endif

static volatile uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

static volatile uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;

static volatile bool rx_overflow = false;
static volatile bool tx_overflow = false;

void UART_Init(void)
{
    uint16_t ubrr = (uint16_t)((F_CPU / (16UL * UART_BAUD)) - 1UL);

    UART_DDR &= ~(1 << UART_RX_PIN);
    UART_DDR |=  (1 << UART_TX_PIN);
    UART_PORT &= ~(1 << UART_RX_PIN);

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    /* USART0, 9600 8N1: async normal speed, 8 data bits, no parity, 1 stop bit. */
    UCSR0A = 0x00;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    rx_head = 0;
    rx_tail = 0;
    tx_head = 0;
    tx_tail = 0;
    rx_overflow = false;
    tx_overflow = false;
}

bool UART_WriteByte(uint8_t byte)
{
    uint8_t next_head = (uint8_t)((tx_head + 1u) & UART_TX_MASK);

    if (next_head == tx_tail)
    {
        tx_overflow = true;
        return false;
    }

    tx_buffer[tx_head] = byte;

    uint8_t sreg = SREG;
    __builtin_avr_cli();
    tx_head = next_head;
    UCSR0B |= (1 << UDRIE0);
    SREG = sreg;

    return true;
}

uint8_t UART_Write(const uint8_t *data, uint8_t length)
{
    uint8_t written = 0;

    while (written < length)
    {
        if (!UART_WriteByte(data[written]))
        {
            break;
        }
        written++;
    }

    return written;
}

void UART_SendChar(char c)
{
    (void)UART_WriteByte((uint8_t)c);
}

void UART_SendString(const char *text)
{
    uint8_t i = 0;

    while (text[i] != '\0')
    {
        if (!UART_WriteByte((uint8_t)text[i]))
        {
            break;
        }
        i++;
    }
}

uint8_t UART_Available(void)
{
    return (rx_head != rx_tail) ? 1u : 0u;
}

bool UART_ReadByte(uint8_t *byte)
{
    if (rx_head == rx_tail)
    {
        return false;
    }

    *byte = rx_buffer[rx_tail];
    rx_tail = (uint8_t)((rx_tail + 1u) & UART_RX_MASK);
    return true;
}

char UART_ReadChar(void)
{
    uint8_t byte = 0;

    if (!UART_ReadByte(&byte))
    {
        return '\0';
    }

    return (char)byte;
}

bool UART_RxOverflowed(void)
{
    return rx_overflow;
}

bool UART_TxOverflowed(void)
{
    return tx_overflow;
}

void UART_ClearOverflowFlags(void)
{
    rx_overflow = false;
    tx_overflow = false;
}

ISR(USART_RX_vect)
{
    uint8_t data = UDR0;
    uint8_t next_head = (uint8_t)((rx_head + 1u) & UART_RX_MASK);

    if (next_head == rx_tail)
    {
        rx_overflow = true;
        return;
    }

    rx_buffer[rx_head] = data;
    rx_head = next_head;
}

ISR(USART_UDRE_vect)
{
    if (tx_head == tx_tail)
    {
        UCSR0B &= ~(1 << UDRIE0);
        return;
    }

    UDR0 = tx_buffer[tx_tail];
    tx_tail = (uint8_t)((tx_tail + 1u) & UART_TX_MASK);
}
