#include "app.h"
#include "app_config.h"
#include "app_types.h"
#include "main.h"

QueueHandle_t     g_q_temp;
QueueHandle_t     g_q_vib;
QueueHandle_t     g_q_status;
SemaphoreHandle_t g_i2c_mutex;

static void create_task(TaskFunction_t fn, const char *name,
                        uint16_t stack_words, UBaseType_t prio)
{
    if (xTaskCreate(fn, name, stack_words, NULL, tskIDLE_PRIORITY + prio, NULL) != pdPASS) {
        Error_Handler();
    }
}

void app_init(void)
{
    g_q_temp    = xQueueCreate(1, sizeof(temp_sample_t));
    g_q_vib     = xQueueCreate(1, sizeof(vib_sample_t));
    g_q_status  = xQueueCreate(1, sizeof(sys_status_t));
    g_i2c_mutex = xSemaphoreCreateMutex();

    if (!g_q_temp || !g_q_vib || !g_q_status || !g_i2c_mutex) {
        Error_Handler();
    }

    create_task(app_vibration_task, "vib",     STACK_VIBRATION, PRIO_VIBRATION);
    create_task(app_alarm_task,     "alarm",   STACK_ALARM,     PRIO_ALARM);
    create_task(app_temp_task,      "temp",    STACK_TEMP,      PRIO_TEMP);
    create_task(app_display_task,   "display", STACK_DISPLAY,   PRIO_DISPLAY);
    create_task(app_uart_task,      "uart",    STACK_UART,      PRIO_UART);
}
