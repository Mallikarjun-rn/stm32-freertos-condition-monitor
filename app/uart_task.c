#include "app.h"
#include "app_config.h"
#include "app_types.h"
#include "alarm_logic.h"
#include "fmt_utils.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef APP_UART_HANDLE;

static void uart_send(const char *s)
{
    HAL_UART_Transmit(&APP_UART_HANDLE, (uint8_t *)s, (uint16_t)strlen(s), 100);
}

/* Output (CSV, one line per second):
 *   t_ms,temp_c,vib_rms_g,state,fault
 * fault bit0 = temperature sensor invalid, bit1 = vibration sensor invalid */
void app_uart_task(void *arg)
{
    (void)arg;

    sys_status_t s;
    char line[80], tbuf[16], vbuf[16];

    uart_send("t_ms,temp_c,vib_rms_g,state,fault\r\n");

    TickType_t last = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(UART_TASK_PERIOD_MS));

#ifdef STATUS_LED_Pin
        HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);   /* heartbeat */
#endif

        if (xQueuePeek(g_q_status, &s, 0) != pdTRUE) continue;

        fmt_fixed(tbuf, sizeof tbuf, s.temp_c, 1);
        fmt_fixed(vbuf, sizeof vbuf, s.vib_rms_g, 3);

        unsigned fault = (s.temp_valid ? 0u : 1u) | (s.vib_valid ? 0u : 2u);

        snprintf(line, sizeof line, "%lu,%s,%s,%s,%u\r\n",
                 (unsigned long)s.uptime_ms, tbuf, vbuf,
                 alarm_state_name(s.state), fault);
        uart_send(line);
    }
}
