#include "dsp_utils.h"
#include <math.h>

static float mean_of(const float *x, uint32_t n)
{
    float s = 0.0f;
    for (uint32_t i = 0; i < n; i++) s += x[i];
    return s / (float)n;
}

float dsp_rms_ac(const float *x, uint32_t n)
{
    if (n == 0u) return 0.0f;
    float m = mean_of(x, n);
    float acc = 0.0f;
    for (uint32_t i = 0; i < n; i++) {
        float d = x[i] - m;
        acc += d * d;
    }
    return sqrtf(acc / (float)n);
}

float dsp_peak_ac(const float *x, uint32_t n)
{
    if (n == 0u) return 0.0f;
    float m = mean_of(x, n);
    float peak = 0.0f;
    for (uint32_t i = 0; i < n; i++) {
        float d = fabsf(x[i] - m);
        if (d > peak) peak = d;
    }
    return peak;
}

void mavg_init(mavg_t *m, float *storage, uint16_t len)
{
    m->buf = storage;
    m->len = len;
    m->idx = 0;
    m->count = 0;
    m->sum = 0.0f;
}

float mavg_update(mavg_t *m, float x)
{
    if (m->count == m->len) {
        m->sum -= m->buf[m->idx];
    } else {
        m->count++;
    }
    m->buf[m->idx] = x;
    m->sum += x;
    m->idx = (uint16_t)((m->idx + 1u) % m->len);
    return m->sum / (float)m->count;
}
