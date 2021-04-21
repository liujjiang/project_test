
#ifndef __LED_H
#define __LED_H
#include "config.h"

void LED_Init(void);
void LED_task(void *pvParameters);

#define LED_B GPIO_Pin_13
#define LED_port GPIOC


#endif
