#include "ir_sensor.h"
#include "adc.h"
#include "pins.h"
#include <stdint.h>

/*
 * Lookup table: ADC value → distance in mm
 * Sharp GP2Y0A21 — derived from datasheet voltage curve at 5V / 10-bit ADC
 * Each entry: { adc_threshold, distance_mm }
 * Table is ordered HIGH adc → LOW adc (close → far)
 * ADC value high = object close, ADC value low = object far
 */
typedef struct {
    uint16_t adc;
    uint16_t dist_mm;
} IR_LookupEntry_t;

static const IR_LookupEntry_t ir_table[] = {
    { 640, 100 },
    { 560, 120 },
    { 490, 140 },
    { 430, 160 },
    { 385, 180 },
    { 350, 200 },
    { 315, 230 },
    { 285, 260 },
    { 260, 300 },
    { 235, 340 },
    { 215, 380 },
    { 200, 420 },
    { 185, 460 },
    { 173, 500 },
    { 163, 540 },
    { 155, 580 },
    { 148, 620 },
    { 141, 660 },
    { 136, 700 },
    { 131, 740 },
    { 127, 800 },
};

#define IR_TABLE_SIZE  (sizeof(ir_table) / sizeof(ir_table[0]))

/*
 * adc_to_mm — interpolate distance from raw ADC value
 * Returns 0 if out of range
 */
static uint16_t adc_to_mm(uint16_t adc)
{
    /* Too close — above highest ADC entry */
    if (adc >= ir_table[0].adc)
    {
        return ir_table[0].dist_mm;
    }

    /* Too far — below lowest ADC entry */
    if (adc <= ir_table[IR_TABLE_SIZE - 1].adc)
    {
        return 0;
    }

    /* Find bracket and linear interpolate */
    for (uint8_t i = 0; i < IR_TABLE_SIZE - 1; i++)
    {
        if (adc <= ir_table[i].adc && adc >= ir_table[i + 1].adc)
        {
            uint16_t adc_high  = ir_table[i].adc;
            uint16_t adc_low   = ir_table[i + 1].adc;
            uint16_t dist_near = ir_table[i].dist_mm;
            uint16_t dist_far  = ir_table[i + 1].dist_mm;

            /* Linear interpolation */
            uint16_t dist = dist_near +
                (uint16_t)((uint32_t)(dist_far - dist_near) *
                            (adc_high - adc) /
                            (adc_high - adc_low));
            return dist;
        }
    }

    return 0;
}

/*
 * take 3 samples, return median — rejects single spikes
 */
static uint16_t adc_median(uint8_t channel)
{
    uint16_t a = ADC_Read(channel);
    uint16_t b = ADC_Read(channel);
    uint16_t c = ADC_Read(channel);

    /* Simple 3-sample median */
    if (a > b) { uint16_t t = a; a = b; b = t; }
    if (b > c) { uint16_t t = b; b = c; c = t; }
    if (a > b) { uint16_t t = a; a = b; b = t; }
    return b; /* middle value */
}

/* ── Public API ─────────────────────────────── */

void IR_Init(void)
{
    ADC_Init();
}

uint16_t IR_GetLeftDistance(void)
{
    uint16_t raw = adc_median(IR_LEFT_ADC_CH);
    return adc_to_mm(raw);
}

uint16_t IR_GetRightDistance(void)
{
    uint16_t raw = adc_median(IR_RIGHT_ADC_CH);
    return adc_to_mm(raw);
}