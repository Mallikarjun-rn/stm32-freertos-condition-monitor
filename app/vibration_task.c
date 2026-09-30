#include "app.h"
#include "app_config.h"
#include "app_types.h"
#include "dsp_utils.h"
#include "mpu6050.h"
#include "main.h"
#include <math.h>

extern I2C_HandleTypeDef hi2c1;

void app_vibration_task(void *arg)
{
    (void)arg;

    static float mag[VIB_WINDOW_SAMPLES];
    uint32_t n = 0;
    uint32_t fails = 0;
    vib_sample_t out = { 0.0f, 0.0f, false, 0 };

    /* Bring up the sensor; keep retrying and report "invalid" meanwhile. */
    for (;;) {
        bool ok = false;
        if (xSemaphoreTake(g_i2c_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            ok = (mpu6050_init(&hi2c1) == HAL_OK);
            xSemaphoreGive(g_i2c_mutex);
        }
        if (ok) break;

        out.valid   = false;
        out.tick_ms = APP_TICK_MS();
        xQueueOverwrite(g_q_vib, &out);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    TickType_t last = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(VIB_TASK_PERIOD_MS));

        float ax = 0, ay = 0, az = 0;
        HAL_StatusTypeDef st = HAL_ERROR;

        /* Short timeout: if the OLED is holding the bus, drop this sample
         * rather than blocking (see docs/architecture.md, "I2C timing"). */
        if (xSemaphoreTake(g_i2c_mutex, pdMS_TO_TICKS(5)) == pdTRUE) {
            st = mpu6050_read_accel_g(&hi2c1, &ax, &ay, &az);
            xSemaphoreGive(g_i2c_mutex);
        }

        if (st != HAL_OK) {
            if (++fails == VIB_FAULT_LIMIT) {
                out.valid   = false;
                out.tick_ms = APP_TICK_MS();
                xQueueOverwrite(g_q_vib, &out);
            }
            continue;
        }
        fails = 0;

        mag[n++] = sqrtf(ax * ax + ay * ay + az * az);

        if (n >= VIB_WINDOW_SAMPLES) {
            out.rms_g   = dsp_rms_ac(mag, n);
            out.peak_g  = dsp_peak_ac(mag, n);
            out.valid   = true;
            out.tick_ms = APP_TICK_MS();
            xQueueOverwrite(g_q_vib, &out);
            n = 0;
        }
    }
}
