#include "main.h"
#include "spi.h"
#include "adns3080.h"
#include "dwt_delay.h"
#include "usbd_cdc_if.h"
#include "gpio.h"

void adns3080_reset(SPI_HandleTypeDef *dev)
{
    if (dev->Instance == SPI2)
    {
        HAL_GPIO_WritePin(SPI2_RST_GPIO_Port, SPI2_RST_Pin, 1);
        dwt_delay_us(64);
        HAL_GPIO_WritePin(SPI2_RST_GPIO_Port, SPI2_RST_Pin, 0);
    }
    else
    {
        HAL_GPIO_WritePin(SPI3_RST_GPIO_Port, SPI3_RST_Pin, 1);
        dwt_delay_us(64);
        HAL_GPIO_WritePin(SPI3_RST_GPIO_Port, SPI3_RST_Pin, 0);
    }
    HAL_Delay(36);

    adns3080_write_reg(dev, A_CONF, 0x19); // 1600cpi
}

void adns3080_init(void)
{
    dwt_delay_init();

    adns3080_reset(&hspi2);
    adns3080_reset(&hspi3);
}

uint8_t adns3080_read_reg(SPI_HandleTypeDef *dev, uint8_t addr)
{
    uint8_t tx_buf[2], rx_buf[2];

    tx_buf[0] = addr;
    HAL_SPI_TransmitReceive(dev, tx_buf, rx_buf, 1, 10);

    dwt_delay_us(50);

    tx_buf[0] = 0x00;
    HAL_SPI_TransmitReceive(dev, tx_buf, rx_buf, 1, 10);

    // __HAL_SPI_DISABLE(&hspi2);
    return rx_buf[0];
}

void adns3080_write_reg(SPI_HandleTypeDef *dev, uint8_t addr, uint8_t val)
{
    uint8_t tx_buf[2], rx_buf[2];

    tx_buf[0] = addr | 0x80;
    HAL_SPI_TransmitReceive(dev, tx_buf, rx_buf, 1, 10);

    dwt_delay_us(50);

    tx_buf[0] = val;
    HAL_SPI_TransmitReceive(dev, tx_buf, rx_buf, 1, 10);

    // __HAL_SPI_DISABLE(&hspi2);
}

void adns3080_motion(SPI_HandleTypeDef *dev, motion_t *motion)
{
    uint8_t tx_buf[2], rx_buf[8];

    tx_buf[0] = A_MOTION_BURST;
    HAL_SPI_TransmitReceive(dev, tx_buf, rx_buf, 1, 10);
    dwt_delay_us(75);

    HAL_SPI_Receive(dev, rx_buf, 7, 10);
    __HAL_SPI_DISABLE(dev);

    memcpy(motion, rx_buf, 7);

    // ADNS3080_CS_L; asm("nop\n"); asm("nop\n");

    // spi_writeread(&ADNS3080_SPI, A_MOTION_BURST); _delay_us(50);

    // *motion = spi_writeread(&ADNS3080_SPI, 0x00);
    // *dx = spi_writeread(&ADNS3080_SPI, 0x00);
    // *dy = spi_writeread(&ADNS3080_SPI, 0x00);
    // *squal = spi_writeread(&ADNS3080_SPI, 0x00);
    // *shutter_upper = spi_writeread(&ADNS3080_SPI, 0x00);
    // *shutter_lower = spi_writeread(&ADNS3080_SPI, 0x00);
    // *max_pixel = spi_writeread(&ADNS3080_SPI, 0x00);

    // ADNS3080_CS_H;
    // _delay_us(4);
}

void adns3080_capture(SPI_HandleTypeDef *dev)
{
    uint8_t start = 0;
    uint8_t reg = 0x00;

    uint8_t img_buf[1024];

    uint8_t tx_buf[2], rx_buf[2];

    __HAL_SPI_DISABLE(dev);
    dwt_delay_us(1);
    __HAL_SPI_ENABLE(dev);
    dwt_delay_us(1);

    adns3080_write_reg(dev, A_FRAME_CAPTURE, 0x83);
    __HAL_SPI_DISABLE(dev);
    dwt_delay_us(1024); // !!!!

    tx_buf[0] = A_PIXEL_BURST;
    HAL_SPI_TransmitReceive(dev, tx_buf, rx_buf, 1, 10);
    dwt_delay_us(60);

    uint32_t count = 0;
    for (uint32_t i = 0;;)
    {
        tx_buf[0] = 0x00;
        HAL_SPI_TransmitReceive(dev, tx_buf, rx_buf, 1, 10);
        reg = rx_buf[0];

        // tx_buf[0] = reg;
        // tx_buf[1] = '\n';
        // CDC_Transmit_FS(tx_buf, 2);

        if (!start && (reg & 0x80) && (reg & 0x40))
        {
            start = 1;
            reg <<= 2;

            img_buf[count] = reg;
            count++;
        }
        else if (start)
        {
            reg <<= 2;

            img_buf[count] = reg;
            count++;

            if (i >= 900)
            {
                // printf("\n****\n");
                break;
            }
        }
        else
        {
            if (i > 64)
            {
                CDC_Transmit_FS("failed to find header\n", 22);
                return;
            }
        }

        i++;
    }

    __HAL_SPI_DISABLE(dev);
    dwt_delay_us(10);

    CDC_Transmit_FS(img_buf, 900);
}
