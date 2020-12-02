#ifndef _SER_IMU_H
#define _SER_IMU_H

#include "main.h"
#include "usart.h"

#define SER_IMU_RX_BUFF_SIZE 128

static uint8_t ser_imu_rx_buff[];
extern uint8_t ser_imu_frame_flag;


void ser_imu_tick(void);
void ser_imu_init(void);
void ser_imu_get(uint16_t *data);


#endif