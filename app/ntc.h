#ifndef NTC_H
#define NTC_H

#include <stdint.h>

#define NTC_INVALID_C  (-999.0f)

/* Convert an ADC reading to degrees C for an NTC thermistor wired as
 *   Vcc - r_fixed - (ADC node) - NTC - GND
 * using the Beta equation. Returns NTC_INVALID_C for open/short readings
 * (adc == 0 or adc >= adc_max). */
float ntc_adc_to_celsius(uint16_t adc, uint16_t adc_max,
                         float r_fixed, float r0, float beta, float t0_c);

#endif /* NTC_H */
