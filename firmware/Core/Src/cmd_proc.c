#include "cmd_proc.h"

#include "usbd_cdc_if.h"
#include "rtc.h"
#include "spi.h"
#include "i2c.h"

#include "adns3080.h"

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

        buf_len = snprintf(buf, 32, "before: 0x%d | ", HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR25));
        CDC_Transmit_FS(buf, buf_len);

        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x5a5a);

        buf_len = snprintf(buf, 32, "after: 0x%d \n", HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR25));
        CDC_Transmit_FS(buf, buf_len);

        // HAL_NVIC_SystemReset();
    }
    else if (!strncmp(cmd, "imu", 3))
    {
        HAL_StatusTypeDef hal_rev;
        hal_rev = HAL_I2C_Mem_Read(&hi2c1, 0xD5, 0xf, I2C_MEMADD_SIZE_8BIT, rx_reg, 1, 100);
        
        uint32_t errno = HAL_I2C_GetError(&hi2c1);
        buf_len = snprintf(buf, 64, "rev: %d, errno: %d,imu: 0x%02x\n", hal_rev, errno, rx_reg[0]);
        CDC_Transmit_FS(buf, buf_len);

        hal_rev = HAL_I2C_Mem_Read(&hi2c1, 0x39, 0xf, I2C_MEMADD_SIZE_8BIT, rx_reg, 1, 100);
        
        errno = HAL_I2C_GetError(&hi2c1);
        buf_len = snprintf(buf, 64, "rev: %d, errno: %d,imu: 0x%02x\n", hal_rev, errno, rx_reg[0]);
        CDC_Transmit_FS(buf, buf_len);
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