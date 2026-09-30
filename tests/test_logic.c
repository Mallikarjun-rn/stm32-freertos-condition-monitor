/* Host-side unit tests for the hardware-independent modules.
 * Build & run:
 *   gcc -std=c99 -Wall -Wextra -Iapp tests/test_logic.c \
 *       app/alarm_logic.c app/dsp_utils.c app/ntc.c app/fmt_utils.c -lm -o test_logic
 *   ./test_logic
 */
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "alarm_logic.h"
#include "dsp_utils.h"
#include "fmt_utils.h"
#include "ntc.h"

static int g_fail = 0;
static int g_run  = 0;

#define CHECK(cond) do { \
    g_run++; \
    if (!(cond)) { g_fail++; printf("FAIL  %s:%d  %s\n", __FILE__, __LINE__, #cond); } \
} while (0)

#define CHECK_NEAR(a, b, tol) CHECK(fabsf((float)(a) - (float)(b)) <= (tol))

static void test_alarm(void)
{
    const float W = 60.0f, C = 80.0f, H = 3.0f;

    CHECK(alarm_evaluate(STATE_NORMAL, 59.0f, W, C, H) == STATE_NORMAL);
    CHECK(alarm_evaluate(STATE_NORMAL, 60.0f, W, C, H) == STATE_WARNING);
    CHECK(alarm_evaluate(STATE_NORMAL, 85.0f, W, C, H) == STATE_CRITICAL);

    /* hysteresis: stays in WARNING until below warn - hyst */
    CHECK(alarm_evaluate(STATE_WARNING, 58.0f, W, C, H) == STATE_WARNING);
    CHECK(alarm_evaluate(STATE_WARNING, 56.9f, W, C, H) == STATE_NORMAL);

    /* hysteresis: stays CRITICAL until below crit - hyst */
    CHECK(alarm_evaluate(STATE_CRITICAL, 78.0f, W, C, H) == STATE_CRITICAL);
    CHECK(alarm_evaluate(STATE_CRITICAL, 76.0f, W, C, H) == STATE_WARNING);
    CHECK(alarm_evaluate(STATE_CRITICAL, 50.0f, W, C, H) == STATE_NORMAL);

    CHECK(alarm_worst(STATE_NORMAL, STATE_WARNING) == STATE_WARNING);
    CHECK(alarm_worst(STATE_CRITICAL, STATE_WARNING) == STATE_CRITICAL);
    CHECK(strcmp(alarm_state_name(STATE_CRITICAL), "CRITICAL") == 0);
}

static void test_dsp(void)
{
    const float flat[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    const float alt[4]   = { 1.0f, -1.0f, 1.0f, -1.0f };
    const float off[4]   = { 2.0f, 0.0f, 2.0f, 0.0f };   /* DC offset of 1 */

    CHECK_NEAR(dsp_rms_ac(flat, 4), 0.0f, 1e-6f);
    CHECK_NEAR(dsp_rms_ac(alt, 4),  1.0f, 1e-6f);
    CHECK_NEAR(dsp_rms_ac(off, 4),  1.0f, 1e-6f);        /* DC removed */
    CHECK_NEAR(dsp_peak_ac(off, 4), 1.0f, 1e-6f);
    CHECK_NEAR(dsp_rms_ac(alt, 0),  0.0f, 1e-6f);

    float storage[3];
    mavg_t m;
    mavg_init(&m, storage, 3);
    CHECK_NEAR(mavg_update(&m, 3.0f),  3.0f, 1e-5f);
    CHECK_NEAR(mavg_update(&m, 6.0f),  4.5f, 1e-5f);
    CHECK_NEAR(mavg_update(&m, 9.0f),  6.0f, 1e-5f);
    CHECK_NEAR(mavg_update(&m, 12.0f), 9.0f, 1e-5f);     /* (6+9+12)/3 */
}

static void test_ntc(void)
{
    /* mid-scale ADC with 10k/10k divider => ~25 C */
    float t = ntc_adc_to_celsius(2048, 4095, 10000.0f, 10000.0f, 3950.0f, 25.0f);
    CHECK_NEAR(t, 25.0f, 0.1f);

    /* NTC to GND: lower ADC counts (lower NTC resistance) => hotter */
    float hot  = ntc_adc_to_celsius(1000, 4095, 10000.0f, 10000.0f, 3950.0f, 25.0f);
    float cold = ntc_adc_to_celsius(3000, 4095, 10000.0f, 10000.0f, 3950.0f, 25.0f);
    CHECK(hot > 25.0f);
    CHECK(cold < 25.0f);

    /* open / shorted sensor */
    CHECK(ntc_adc_to_celsius(0,    4095, 10000.0f, 10000.0f, 3950.0f, 25.0f) == NTC_INVALID_C);
    CHECK(ntc_adc_to_celsius(4095, 4095, 10000.0f, 10000.0f, 3950.0f, 25.0f) == NTC_INVALID_C);
}

static void test_fmt(void)
{
    char b[16];
    fmt_fixed(b, sizeof b, -3.14159f, 2); CHECK(strcmp(b, "-3.14") == 0);
    fmt_fixed(b, sizeof b, 0.0456f, 3);   CHECK(strcmp(b, "0.046") == 0);
    fmt_fixed(b, sizeof b, 12.34f, 1);    CHECK(strcmp(b, "12.3") == 0);
    fmt_fixed(b, sizeof b, 7.0f, 0);      CHECK(strcmp(b, "7") == 0);
    fmt_fixed(b, sizeof b, -0.001f, 2);   CHECK(strcmp(b, "0.00") == 0);
}

int main(void)
{
    test_alarm();
    test_dsp();
    test_ntc();
    test_fmt();

    printf("%d checks, %d failed\n", g_run, g_fail);
    return g_fail ? 1 : 0;
}
