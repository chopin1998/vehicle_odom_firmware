#ifndef _SER_IMU_H
#define _SER_IMU_H

#include "main.h"
#include "usart.h"

#define SER_IMU_RX_BUFF_SIZE 128

static uint8_t ser_imu_rx_buff[];
extern uint8_t ser_imu_frame_flag;
extern uint8_t ser_imu_last_data[32];

void ser_imu_tick(void);
void ser_imu_init(void);
void ser_imu_get(int16_t *a, int16_t *r, int16_t *e);


#endif