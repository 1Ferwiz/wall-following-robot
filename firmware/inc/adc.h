#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/*
 * ADC_Init — configure ADC hardware once at startup
 * Call before any ADC_Read()
 */
void ADC_Init(void);

/*
 * ADC_Read — perform a single blocking conversion
 * channel: 0–7  (use IR_LEFT_ADC_CH / IR_RIGHT_ADC_CH from pins.h)
 * returns: 10-bit result (0–1023)
 */
uint16_t ADC_Read(uint8_t channel);

#endif /* ADC_H */