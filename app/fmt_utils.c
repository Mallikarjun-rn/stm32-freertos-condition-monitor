#include "fmt_utils.h"
#include <stdio.h>

int fmt_fixed(char *out, size_t n, float v, uint8_t decimals)
{
    static const char *const fmts[] = {
        "%s%lu", "%s%lu.%01lu", "%s%lu.%02lu", "%s%lu.%03lu"
    };
    static const uint32_t scales[] = { 1u, 10u, 100u, 1000u };

    if (decimals > 3u) decimals = 3u;

    int neg = (v < 0.0f);
    if (neg) v = -v;

    uint32_t scale = scales[decimals];
    uint32_t x = (uint32_t)(v * (float)scale + 0.5f);

    return snprintf(out, n, fmts[decimals],
                    (neg && x != 0u) ? "-" : "",
                    (unsigned long)(x / scale),
                    (unsigned long)(x % scale));
}
