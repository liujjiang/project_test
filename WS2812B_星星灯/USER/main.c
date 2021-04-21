#include "config.h"


void start_task(void *pvParameters);

int main(void)
{
    task_Init();

	//������ʼ����
    xTaskCreate(start_task,"start_task",128,NULL,1,NULL);
	
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���

    xTaskCreate(LED_task     		 , "LED_task" 		    , 64   	   , NULL , 2 , NULL);
    xTaskCreate(WS2812B_task     , "WS2812B_task" 		, 128   	 , NULL , 1 , NULL);


	
    vTaskDelete(NULL);              //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


