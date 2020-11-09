#ifndef _IMU_H
#define _IMU_H

#include "main.h"
#include "i2c.h"

#include "lsm9ds1_reg.h"
#include "motion_fx.h"

typedef union
{
    int16_t i16bit[3];
    uint8_t u8bit[6];
} axis3bit16_t;

extern stmdev_ctx_t dev_imu, dev_mag;

volatile static uint8_t imu_xl_gy_drdy_flag=0, imu_mag_drdy_flag=0;
void imu_xl_gy_drdy(void);
void imu_mag_drdy(void);

uint8_t is_imu_xl_gy_drdy(void);
uint8_t is_imu_mag_drdy(void);

uint32_t imu_init(void);

void imu_getdata(MFX_input_t *input);

#endif