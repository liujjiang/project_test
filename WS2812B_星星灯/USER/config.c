#include "config.h"

void task_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 	 
	uart_init(115200);
	delay_init();	    			  
	LED_Init();	 
//	DS1302_Init();
	
	bsp_InitSPIBus();	/* 配置SPI总线 */
	
  all_off_2812();
}



