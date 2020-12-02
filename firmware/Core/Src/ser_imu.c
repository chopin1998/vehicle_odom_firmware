#include "ser_imu.h"
#include "usart.h"
#include "string.h"

static uint8_t ser_imu_rx_buff[SER_IMU_RX_BUFF_SIZE];

static uint8_t ser_imu_last_euler[6];

uint8_t ser_imu_frame_flag = 0;

// void ser_imu_tick(void)
// {
// }

void ser_imu_enable(void)
{
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart3, ser_imu_rx_buff, SER_IMU_RX_BUFF_SIZE);
}

void ser_imu_callback(uint16_t data_len)
{
    // CDC_Transmit_FS(ser_imu_rx_buff, data_len);
    // HAL_GPIO_TogglePin(LED_0_GPIO_Port, LED_0_Pin);

    if (data_len != 13)
    {
        return;
    }

    ser_imu_frame_flag = 1;
    
    memcpy(ser_imu_last_euler, ser_imu_rx_buff+7, 6);

    // uint8_t tmp;
    // tmp = ser_imu_last_euler[0];
    // ser_imu_last_euler[0] = ser_imu_last_euler[1];
    // ser_imu_last_euler[1] = tmp;

    // tmp = ser_imu_last_euler[2];
    // ser_imu_last_euler[2] = ser_imu_last_euler[3];
    // ser_imu_last_euler[3] = tmp;
    
    // tmp = ser_imu_last_euler[4];
    // ser_imu_last_euler[4] = ser_imu_last_euler[5];
    // ser_imu_last_euler[5] = tmp;    
}

void ser_imu_get(uint16_t* data)
{
    memcpy(data, &ser_imu_last_euler, 6);
}