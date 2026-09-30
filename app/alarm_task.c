#include "app.h"
#include "app_config.h"
#include "app_types.h"
#include "alarm_logic.h"
#include "main.h"

/* Requires CubeMX GPIO user labels: ALARM_LED and BUZZER (outputs, active high). */

static void set_outputs(sys_state_t state, bool *blink)
{
    switch (state) {
    case STATE_CRITICAL:                       /* fast blink + beep */
        *blink = !*blink;
        HAL_GPIO_WritePin(ALARM_LED_GPIO_Port, ALARM_LED_Pin, *blink ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BUZZER_GPIO_Port,    BUZZER_Pin,    *blink ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    case STATE_WARNING:                        /* LED solid, buzzer off */
        HAL_GPIO_WritePin(ALARM_LED_GPIO_Port, ALARM_LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BUZZER_GPIO_Port,    BUZZER_Pin,    GPIO_PIN_RESET);
        break;
    case STATE_NORMAL:
    default:
        HAL_GPIO_WritePin(ALARM_LED_GPIO_Port, ALARM_LED_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BUZZER_GPIO_Port,    BUZZER_Pin,    GPIO_PIN_RESET);
        break;
    }
}

void app_alarm_task(void *arg)
{
    (void)arg;

    sys_state_t   ts = STATE_NORMAL, vs = STATE_NORMAL;
    temp_sample_t t  = { 0 };
    vib_sample_t  v  = { 0 };
    sys_status_t  st = { 0 };
    bool blink = false;

    TickType_t last = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(ALARM_TASK_PERIOD_MS));

        st.temp_valid = (xQueuePeek(g_q_temp, &t, 0) == pdTRUE) && t.valid;
        st.vib_valid  = (xQueuePeek(g_q_vib,  &v, 0) == pdTRUE) && v.valid;

        if (st.temp_valid) {
            st.temp_c = t.temp_c;
            ts = alarm_evaluate(ts, t.temp_c, TEMP_WARN_C, TEMP_CRIT_C, TEMP_HYST_C);
        }
        if (st.vib_valid) {
            st.vib_rms_g = v.rms_g;
            vs = alarm_evaluate(vs, v.rms_g, VIB_WARN_G, VIB_CRIT_G, VIB_HYST_G);
        }

        st.temp_state = ts;
        st.vib_state  = vs;
        st.state      = alarm_worst(ts, vs);

        /* Fail-safe: a dead sensor must never look like "all normal". */
        if (!st.temp_valid || !st.vib_valid) {
            st.state = alarm_worst(st.state, STATE_WARNING);
        }

        st.uptime_ms = APP_TICK_MS();
        xQueueOverwrite(g_q_status, &st);

        set_outputs(st.state, &blink);
    }
}
