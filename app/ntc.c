#include "ntc.h"
#include <math.h>

float ntc_adc_to_celsius(uint16_t adc, uint16_t adc_max,
                         float r_fixed, float r0, float beta, float t0_c)
{
    if (adc == 0u || adc >= adc_max) return NTC_INVALID_C;

    float r_ntc = r_fixed * (float)adc / (float)(adc_max - adc);
    float t0_k  = t0_c + 273.15f;
    float inv_t = (1.0f / t0_k) + (logf(r_ntc / r0) / beta);
    return (1.0f / inv_t) - 273.15f;
}
