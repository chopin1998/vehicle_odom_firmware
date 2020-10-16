#include "cmd_proc.h"
#include "usbd_cdc_if.h"


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
    else if (!strncmp(cmd, "reg", 2))
    {
        adns3080_init();

        buf_len = snprintf(buf, 64, "id: 0x%02x\n", adns3080_read_reg(A_PIV_INVERSE));
        CDC_Transmit_FS(buf, buf_len);
    }
    else if (!strncmp(cmd, "boot", 4))
    {
        RTC->BKP0R = 0x5a;

        HAL_NVIC_SystemReset();
    }
    // else if (!strncmp(cmd, "imu", 3))
    // {
    //     HAL_StatusTypeDef hal_rev;

    //     hal_rev = HAL_I2C_Mem_Read(&hi2c1, 0xD5, 0xf, I2C_MEMADD_SIZE_8BIT, rx_reg, 1, 100000);

    //     uint32_t errno = HAL_I2C_GetError(&hi2c1);

    //     buf_len = snprintf(buf, 64, "rev: %d, errno: %d,imu: 0x%02x\n", hal_rev, errno, rx_reg[0]);
    //     CDC_Transmit_FS(buf, buf_len);
    // }
    else
    {
        CDC_Transmit_FS(cmd, len);
    }
}