#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <stdint.h>

void UART_Init(void);

bool UART_WriteByte(uint8_t byte);
uint8_t UART_Write(const uint8_t *data, uint8_t length);
void UART_SendChar(char c);
void UART_SendString(const char *text);
void UART_SendUInt(uint16_t val);
void UART_SendNewline(void);
void UART_SendTurnReport(uint8_t count, const char *sequence);

uint8_t UART_Available(void);
bool UART_ReadByte(uint8_t *byte);
char UART_ReadChar(void);

bool UART_RxOverflowed(void);
bool UART_TxOverflowed(void);
void UART_ClearOverflowFlags(void);

#endif /* UART_H */
