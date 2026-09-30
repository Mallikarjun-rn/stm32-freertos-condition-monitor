#ifndef FMT_UTILS_H
#define FMT_UTILS_H

#include <stddef.h>
#include <stdint.h>

/* Print a float with 0..3 decimals without needing float support in
 * newlib-nano printf ("-u _printf_float"). Range: |v| < ~4e6 / 10^decimals. */
int fmt_fixed(char *out, size_t n, float v, uint8_t decimals);

#endif /* FMT_UTILS_H */
