#ifndef DSP_UTILS_H
#define DSP_UTILS_H

#include <stdint.h>

/* AC RMS: RMS of (x - mean). Removes the DC offset (e.g. 1 g of gravity). */
float dsp_rms_ac(const float *x, uint32_t n);

/* Largest absolute deviation from the mean. */
float dsp_peak_ac(const float *x, uint32_t n);

/* Simple moving average over caller-provided storage. */
typedef struct {
    float   *buf;
    uint16_t len;
    uint16_t idx;
    uint16_t count;
    float    sum;
} mavg_t;

void  mavg_init(mavg_t *m, float *storage, uint16_t len);
float mavg_update(mavg_t *m, float x);

#endif /* DSP_UTILS_H */
