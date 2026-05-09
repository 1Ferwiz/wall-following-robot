#include "../inc/uart.h"
#include "../config/pins.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#define UBRR_VAL              ((F_CPU / (16UL * UART_BAUD)) - 1UL)
#define UART_RX_BUFFER_SIZE   64u
#define UART_TX_BUFFER_SIZE   192u

static volatile uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;
static volatile bool rx_overflow = false;

static volatile uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;
static volatile bool tx_overflow = false;

static uint8_t next_rx_index(uint8_t index) {
    return (uint8_t)((index + 1u) % UART_RX_BUFFER_SIZE);
}

static uint8_t next_tx_index(uint8_t index) {
    return (uint8_t)((index + 1u) % UART_TX_BUFFER_SIZE);
}

void UART_Init(void) {
    rx_head = 0;
    rx_tail = 0;
    tx_head = 0;
    tx_tail = 0;
    rx_overflow = false;
    tx_overflow = false;

    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)UBRR_VAL;

    UCSR0A = 0;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

ISR(USART_RX_vect) {
    uint8_t data = UDR0;
    uint8_t next = next_rx_index(rx_head);

    if (next == rx_tail) {
        rx_overflow = true;
        return;
    }

    rx_buffer[rx_head] = data;
    rx_head = next;
}

ISR(USART_UDRE_vect) {
    if (tx_tail == tx_head) {
        UCSR0B &= ~(1 << UDRIE0);
        return;
    }

    UDR0 = tx_buffer[tx_tail];
    tx_tail = next_tx_index(tx_tail);
}

bool UART_WriteByte(uint8_t byte) {
    bool queued = false;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        uint8_t next = next_tx_index(tx_head);

        if (next != tx_tail) {
            tx_buffer[tx_head] = byte;
            tx_head = next;
            UCSR0B |= (1 << UDRIE0);
            queued = true;
        } else {
            tx_overflow = true;
        }
    }

    return queued;
}

uint8_t UART_Write(const uint8_t *data, uint8_t length) {
    uint8_t written = 0;

    while (written < length) {
        if (!UART_WriteByte(data[written])) {
            break;
        }
        written++;
    }

    return written;
}

void UART_SendChar(char c) {
    (void)UART_WriteByte((uint8_t)c);
}

void UART_SendString(const char *text) {
    while (*text) {
        UART_SendChar(*text++);
    }
}

uint8_t UART_Available(void) {
    uint8_t available;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        available = (rx_head != rx_tail) ? 1u : 0u;
    }

    return available;
}

bool UART_ReadByte(uint8_t *byte) {
    bool has_byte = false;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (rx_head != rx_tail) {
            *byte = rx_buffer[rx_tail];
            rx_tail = next_rx_index(rx_tail);
            has_byte = true;
        }
    }

    return has_byte;
}

char UART_ReadChar(void) {
    uint8_t byte = 0;

    while (!UART_ReadByte(&byte)) {
    }

    return (char)byte;
}

bool UART_RxOverflowed(void) {
    return rx_overflow;
}

bool UART_TxOverflowed(void) {
    return tx_overflow;
}

void UART_ClearOverflowFlags(void) {
    rx_overflow = false;
    tx_overflow = false;
}

void UART_SendUInt(uint16_t val) {
    char buf[6];
    uint8_t i = 0;

    if (val == 0) {
        UART_SendChar('0');
        return;
    }

    while (val > 0) {
        buf[i++] = (char)('0' + (val % 10u));
        val /= 10u;
    }

    while (i > 0) {
        UART_SendChar(buf[--i]);
    }
}

void UART_SendNewline(void) {
    UART_SendChar('\r');
    UART_SendChar('\n');
}

void UART_SendTurnReport(uint8_t count, const char *sequence) {
    UART_SendString("Turns: ");
    UART_SendUInt(count);
    UART_SendNewline();

    UART_SendString("Sequence: ");
    for (uint8_t i = 0; i < count; i++) {
        UART_SendChar(sequence[i]);
        if (i < (uint8_t)(count - 1u)) {
            UART_SendString(", ");
        }
    }
    UART_SendNewline();
}
