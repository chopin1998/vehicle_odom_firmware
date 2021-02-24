#include "ser_imu.h"
#include "usart.h"
#include "string.h"

static uint8_t ser_imu_rx_buff[SER_IMU_RX_BUFF_SIZE];

uint8_t ser_imu_last_data[32];

uint8_t ser_imu_frame_flag = 0;

// void ser_imu_tick(void)
// {
// }

void ser_imu_enable(void)
{
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart2, ser_imu_rx_buff, SER_IMU_RX_BUFF_SIZE);
}

void ser_imu_callback(uint16_t data_len)
{
    // HAL_GPIO_TogglePin(LED_0_GPIO_Port, LED_0_Pin);

    if (data_len != 27)
    {
        CDC_Transmit_FS(ser_imu_rx_buff, data_len);
        return;
    }

    // CDC_Transmit_FS(ser_imu_rx_buff, data_len);
    ser_imu_frame_flag = 1;
    memcpy(ser_imu_last_data, ser_imu_rx_buff+6, 21);
}

void ser_imu_get(int16_t* a, int16_t* g, int16_t* eular)
{
    memcpy(a, ser_imu_last_data+1, 6);
    memcpy(g, ser_imu_last_data+8, 6);
    memcpy(eular, ser_imu_last_data+15, 6);
}