#include "cmd_proc.h"

#include "usbd_cdc_if.h"
#include "rtc.h"
#include "spi.h"
#include "i2c.h"

#include "adns3080.h"
#include "motion_fx.h"
#include "lsm9ds1_reg.h"
#include "imu.h"

void cmd_proc(uint8_t *cmd, int len)
{
    // CDC_Transmit_FS(buf, len);
    uint8_t rx_reg[32];
    uint8_t buf[64];
    int buf_len;

    if (!strncmp(cmd, "mon", 3))
    {
        motion_t m;
        adns3080_motion(&m);

        buf_len = snprintf(buf, 48, "0x%02x,%d,%d,%u,%u,%u,%u\n",
                           m.motion, m.dx, m.dy, m.squal, m.shutter_upper, m.shutter_lower, m.max_pixel);

        CDC_Transmit_FS(buf, buf_len);
    }
    else if (!strncmp(cmd, "cap", 3))
    {
        // CDC_Transmit_FS("test\n", 5);
        adns3080_capture();
    }
    else if (!strncmp(cmd, "test", 4))
    {
        // buf_len = MotionFX_GetLibVersion(buf);
        // CDC_Transmit_FS(buf, buf_len);
        // CDC_Transmit_FS("\n", 1);

        // MFX_input_t raw;
        // MFX_output_t out;
        // // imu_getdata(&raw);

        // float dt = 0.01;
        // MotionFX_propagate(&out, &raw, &dt);
    }
    else if (!strncmp(cmd, "imu", 3))
    {
        lsm9ds1_id_t whoami;
        lsm9ds1_dev_id_get(&dev_mag, &dev_imu, &whoami);
        if (whoami.imu != LSM9DS1_IMU_ID || whoami.mag != LSM9DS1_MAG_ID)
        {
            CDC_Transmit_FS("dev not mount\n", 14);
        }
        else
        {
            CDC_Transmit_FS("match\n", 6);
        }
    }
    else if (!strncmp(cmd, "scan", 4))
    {
        // HAL_StatusTypeDef hal_rev;

        // hal_rev = HAL_I2C_Mem_Read(&hi2c1, 0xD6, 0xf, I2C_MEMADD_SIZE_8BIT, rx_reg, 1, 1000);
        for (uint8_t i = 0; i < 255; i++)
        {
            if (HAL_I2C_IsDeviceReady(&hi2c1, i, 10, 100) != HAL_OK)
            {
                HAL_Delay(1);
            }
            else
            {
                buf_len = snprintf(buf, 64, "found dev @0x%02X\n", i);
                CDC_Transmit_FS(buf, buf_len);
            }
        }
    }
    else
    {
        CDC_Transmit_FS(cmd, len);
    }
}