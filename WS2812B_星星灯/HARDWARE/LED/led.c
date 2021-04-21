
#include "led.h"

void LED_Mode_Select(void)
{
	
		        
}
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );
    
    GPIO_InitStructure.GPIO_Pin = LED_B ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_port, &GPIO_InitStructure);
	  GPIO_SetBits(LED_port,LED_B);
}

void circle_flashing(u8 num)
{
	u8 i;
	
	if(num>0)
	{
		for(i=1;i<num;i++)	
		{
			GPIO_ResetBits(LED_port,LED_B); vTaskDelay(150);
			GPIO_SetBits(LED_port,LED_B); 	 vTaskDelay(150);
		}
		GPIO_ResetBits(LED_port,LED_B); vTaskDelay(150);
		GPIO_SetBits(LED_port,LED_B);   vTaskDelay(800);
	}

}


void LED_task(void *pvParameters)
{

  	TickType_t xLastWakeTime;  
		const TickType_t xFrequency = 50;
		xLastWakeTime = xTaskGetTickCount();	
    while(1)
    {
       circle_flashing( 2);
      vTaskDelayUntil(&xLastWakeTime, xFrequency);  
    } 
}

