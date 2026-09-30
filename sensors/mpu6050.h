#ifndef MPU6050_H
#define MPU6050_H

#include "main.h"   /* pulls in the HAL headers */

/* Wake the sensor and configure: +/-4 g range, ~184 Hz accel bandwidth,
 * 1 kHz internal sample rate. Returns HAL_OK on success. */
HAL_StatusTypeDef mpu6050_init(I2C_HandleTypeDef *hi2c);

/* Read X/Y/Z acceleration in g (blocking). */
HAL_StatusTypeDef mpu6050_read_accel_g(I2C_HandleTypeDef *hi2c,
                                       float *ax, float *ay, float *az);

#endif /* MPU6050_H */
