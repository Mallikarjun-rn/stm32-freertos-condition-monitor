#include "app.h"
#include "app_config.h"
#include "app_types.h"
#include "dsp_utils.h"
#include "ntc.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1;

/* Average several polled conversions. (Milestone: replace with ADC + DMA.) */
static bool adc_read_avg(uint16_t *out)
{
    uint32_t sum = 0, ok = 0;

    for (uint32_t i = 0; i < ADC_OVERSAMPLE; i++) {
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK) {
            sum += HAL_ADC_GetValue(&hadc1);
            ok++;
        }
        HAL_ADC_Stop(&hadc1);
    }
    if (ok == 0u) return false;

    *out = (uint16_t)(sum / ok);
    return true;
}

void app_temp_task(void *arg)
{
    (void)arg;

#if defined(STM32F103xB)
    HAL_ADCEx_Calibration_Start(&hadc1);   /* STM32F1 ADC needs calibration */
#endif

    static float mavg_storage[TEMP_MAVG_LEN];
    mavg_t avg;
    mavg_init(&avg, mavg_storage, TEMP_MAVG_LEN);

    temp_sample_t out = { 0.0f, false, 0 };
    TickType_t last = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(TEMP_TASK_PERIOD_MS));

        uint16_t raw;
        out.valid = false;

        if (adc_read_avg(&raw)) {
            float c = ntc_adc_to_celsius(raw, (uint16_t)ADC_MAX_COUNTS,
                                         NTC_R_FIXED_OHMS, NTC_R0_OHMS,
                                         NTC_BETA, NTC_T0_C);
            if (c > -273.0f) {              /* not NTC_INVALID_C */
                out.temp_c = mavg_update(&avg, c);
                out.valid  = true;
            }
        }
        out.tick_ms = APP_TICK_MS();
        xQueueOverwrite(g_q_temp, &out);
    }
}
