#include "app.h"
#include "app_config.h"
#include "app_types.h"
#include "alarm_logic.h"
#include "fmt_utils.h"
#include "main.h"
#include "ssd1306.h"          /* afiskon/stm32-ssd1306 - see README */
#include "ssd1306_fonts.h"
#include <stdio.h>
#include <string.h>

static void draw_line(uint8_t y, const char *text)
{
    ssd1306_SetCursor(0, y);
    ssd1306_WriteString((char *)text, Font_7x10, White);
}

void app_display_task(void *arg)
{
    (void)arg;

    sys_status_t s;
    char line[24], num[12];

    if (xSemaphoreTake(g_i2c_mutex, portMAX_DELAY) == pdTRUE) {
        ssd1306_Init();
        xSemaphoreGive(g_i2c_mutex);
    }

    TickType_t last = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(DISPLAY_TASK_PERIOD_MS));

        if (xQueuePeek(g_q_status, &s, 0) != pdTRUE) continue;

        /* Known limitation: a full-frame update holds the I2C bus ~25 ms.
         * See docs/architecture.md for the page-wise improvement. */
        if (xSemaphoreTake(g_i2c_mutex, pdMS_TO_TICKS(100)) != pdTRUE) continue;

        ssd1306_Fill(Black);
        draw_line(0, "COND. MONITOR");

        if (s.temp_valid) fmt_fixed(num, sizeof num, s.temp_c, 1);
        else              strcpy(num, "--");
        snprintf(line, sizeof line, "Temp : %s C", num);
        draw_line(14, line);

        if (s.vib_valid)  fmt_fixed(num, sizeof num, s.vib_rms_g, 3);
        else              strcpy(num, "--");
        snprintf(line, sizeof line, "Vib  : %s g", num);
        draw_line(26, line);

        snprintf(line, sizeof line, "State: %s", alarm_state_name(s.state));
        draw_line(38, line);

        ssd1306_UpdateScreen();
        xSemaphoreGive(g_i2c_mutex);
    }
}
