#include "mpu6050.h"

#define MPU_ADDR            (0x68u << 1)   /* AD0 low; HAL wants the 8-bit address */
#define MPU_TIMEOUT_MS      5u

#define REG_SMPLRT_DIV      0x19u
#define REG_CONFIG          0x1Au
#define REG_ACCEL_CONFIG    0x1Cu
#define REG_ACCEL_XOUT_H    0x3Bu
#define REG_PWR_MGMT_1      0x6Bu
#define REG_WHO_AM_I        0x75u

#define ACCEL_LSB_PER_G     8192.0f        /* +/-4 g range */

static HAL_StatusTypeDef write_reg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t val)
{
    return HAL_I2C_Mem_Write(hi2c, MPU_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
                             &val, 1, MPU_TIMEOUT_MS);
}

HAL_StatusTypeDef mpu6050_init(I2C_HandleTypeDef *hi2c)
{
    uint8_t id = 0;
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(hi2c, MPU_ADDR, REG_WHO_AM_I,
                                            I2C_MEMADD_SIZE_8BIT, &id, 1, MPU_TIMEOUT_MS);
    if (st != HAL_OK) return st;

    /* 0x68 = genuine MPU6050; 0x70 / 0x72 are seen on common clone boards */
    if (id != 0x68u && id != 0x70u && id != 0x72u) return HAL_ERROR;

    if ((st = write_reg(hi2c, REG_PWR_MGMT_1, 0x01u)) != HAL_OK) return st;  /* wake, PLL clock */
    HAL_Delay(50);
    if ((st = write_reg(hi2c, REG_SMPLRT_DIV,   0x00u)) != HAL_OK) return st; /* 1 kHz          */
    if ((st = write_reg(hi2c, REG_CONFIG,       0x01u)) != HAL_OK) return st; /* DLPF ~184 Hz   */
    if ((st = write_reg(hi2c, REG_ACCEL_CONFIG, 0x08u)) != HAL_OK) return st; /* +/-4 g         */
    return HAL_OK;
}

HAL_StatusTypeDef mpu6050_read_accel_g(I2C_HandleTypeDef *hi2c,
                                       float *ax, float *ay, float *az)
{
    uint8_t b[6];
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(hi2c, MPU_ADDR, REG_ACCEL_XOUT_H,
                                            I2C_MEMADD_SIZE_8BIT, b, sizeof b, MPU_TIMEOUT_MS);
    if (st != HAL_OK) return st;

    int16_t rx = (int16_t)((b[0] << 8) | b[1]);
    int16_t ry = (int16_t)((b[2] << 8) | b[3]);
    int16_t rz = (int16_t)((b[4] << 8) | b[5]);

    *ax = (float)rx / ACCEL_LSB_PER_G;
    *ay = (float)ry / ACCEL_LSB_PER_G;
    *az = (float)rz / ACCEL_LSB_PER_G;
    return HAL_OK;
}
