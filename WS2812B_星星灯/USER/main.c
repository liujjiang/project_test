#include "config.h"


void start_task(void *pvParameters);

int main(void)
{
    task_Init();

	//创建开始任务
    xTaskCreate(start_task,"start_task",128,NULL,1,NULL);
	
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区

    xTaskCreate(LED_task     		 , "LED_task" 		    , 64   	   , NULL , 2 , NULL);
    xTaskCreate(WS2812B_task     , "WS2812B_task" 		, 128   	 , NULL , 1 , NULL);


	
    vTaskDelete(NULL);              //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


