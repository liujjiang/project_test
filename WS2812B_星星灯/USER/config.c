#include "config.h"

void task_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 	 
	uart_init(115200);
	delay_init();	    			  
	LED_Init();	 
//	DS1302_Init();
	
	bsp_InitSPIBus();	/* ����SPI���� */
	
  all_off_2812();
}



