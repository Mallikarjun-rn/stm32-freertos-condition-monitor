#ifndef APP_H
#define APP_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

/* "Latest value" queues (length 1, written with xQueueOverwrite,
 * read with xQueuePeek) so consumers always see the newest sample. */
extern QueueHandle_t     g_q_temp;    /* temp_sample_t */
extern QueueHandle_t     g_q_vib;     /* vib_sample_t  */
extern QueueHandle_t     g_q_status;  /* sys_status_t  */

/* Protects I2C1, shared by the accelerometer and the OLED. */
extern SemaphoreHandle_t g_i2c_mutex;

#define APP_TICK_MS()  ((uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS))

/* Create queues, mutex and tasks. Call once before the scheduler starts. */
void app_init(void);

void app_temp_task(void *arg);
void app_vibration_task(void *arg);
void app_alarm_task(void *arg);
void app_display_task(void *arg);
void app_uart_task(void *arg);

#endif /* APP_H */
