#ifndef _CMD_PROC_H
#define _CMD_PROC_H

#include "main.h"

void cmd_proc(uint8_t *buf, int len);

#define CMD_RESET    0x0052 //
#define CMD_BOOTLOAD 0x0053 //


#endif