#include "adc.h"
#include <avr/io.h>

/*
 * ADC_Init
 * - Reference: AVcc (5V)
 * - Prescaler: 128 → 16MHz/128 = 125kHz (within 50–200kHz ADC clock range)
 * - Right-adjust result → read full 10 bits from ADCW
 */
void ADC_Init(void)
{
    /* AVcc reference, right-adjust, channel 0 to start */
    ADMUX  = (1 << REFS0);

    /* Enable ADC, prescaler = 128 */
    ADCSRA = (1 << ADEN)
           | (1 << ADPS2)
           | (1 << ADPS1)
           | (1 << ADPS0);
}

/*
 * ADC_Read
 * Selects channel, starts conversion, waits, returns result.
 * Blocking — only called from sensor layer, never from ISR.
 */
uint16_t ADC_Read(uint8_t channel)
{
    /* Select channel (0–7), keep reference bits */
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    /* Start conversion */
    ADCSRA |= (1 << ADSC);

    /* Wait for conversion complete (ADSC goes low) */
    while (ADCSRA & (1 << ADSC))
    {
        /* intentional busy-wait — only during sensor read, <0.1ms */
    }

    return ADCW; /* read 10-bit result (ADCL + ADCH) */
}