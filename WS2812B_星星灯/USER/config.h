
#ifndef __CONFIG_H
#define __CONFIG_H
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include <stdbool.h>
#include "math.h"
#include "stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

#include "usart.h"
#include "led.h"
#include "led_color.h"
#include "bsp_spi_bus.h"
#include "RGBcode.h"


void task_Init(void);

#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
#define LPF_1_(hz,t,in,out) ((out) += ( 1 / ( 1 + 1 / ( (hz) *6.28f *(t) ) ) ) *( (in) - (out) ))


#endif

