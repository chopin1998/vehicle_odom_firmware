#include "cmd_proc.h"
#include "main.h"

#include "usbd_cdc_if.h"
#include "rtc.h"
#include "spi.h"
// #include "i2c.h"
#include "ser_imu.h"

#include "adns3080.h"
// #include "motion_fx.h"
// #include "motion_ec.h"
// #include "lsm9ds1_reg.h"
// #include "imu.h"

void cmd_proc(uint8_t *cmd, int len)
{
    uint8_t buf[64];
    int buf_len;

    // if (!strncmp(cmd, "mon", 3))
    // {
    //     motion_t m;
    //     adns3080_motion(&m);

    //     buf_len = snprintf(buf, 48, "0x%02x,%d,%d,%u,%u,%u,%u\n",
    //                        m.motion, m.dx, m.dy, m.squal, m.shutter_upper, m.shutter_lower, m.max_pixel);

    //     CDC_Transmit_FS(buf, buf_len);
    // }
    if (!strncmp(cmd, "mode", 4))
    {
        if (len == 4)
        {
            buf_len = snprintf(buf, 64, "mode: %d\n", odom.mode);
            CDC_Transmit_FS(buf, buf_len);
        }
        else
        {
            adns3080_reset(&hspi2);
            adns3080_reset(&hspi3);
            
            odom.mode = cmd[4] - '0';
        }
    }
    else if (!strncmp(cmd, "cap2", 4))
    {
        // adns3080_reset(&hspi3);
        adns3080_capture(&hspi3);
    }
    else if (!strncmp(cmd, "cap", 3))
    {
        // adns3080_reset(&hspi2);
        adns3080_capture(&hspi2);
    }
    else if (!strncmp(cmd, "AT", 2))
    {
        // buf_len = snprintf(buf, 32, "hello\r\n");
        // HAL_UART_Transmit_DMA(&huart2, buf, buf_len);
        CDC_Transmit_FS(cmd, len);
        HAL_UART_Transmit_DMA(&huart2, cmd, len);
        HAL_GPIO_TogglePin(LED_0_GPIO_Port, LED_0_Pin);
    }
    // else if (!strncmp(cmd, "scan", 4))
    // {
    // HAL_StatusTypeDef hal_rev;

    // hal_rev = HAL_I2C_Mem_Read(&hi2c1, 0xD6, 0xf, I2C_MEMADD_SIZE_8BIT, rx_reg, 1, 1000);
    // for (uint8_t i = 0; i < 255; i++)
    // {
    //     if (HAL_I2C_IsDeviceReady(&hi2c1, i, 10, 100) != HAL_OK)
    //     {
    //         HAL_Delay(1);
    //     }
    //     else
    //     {
    //         buf_len = snprintf(buf, 64, "found dev @0x%02X\n", i);
    //         CDC_Transmit_FS(buf, buf_len);
    //     }
    // }
    // }
    else
    {
        CDC_Transmit_FS(cmd, len);
    }
}