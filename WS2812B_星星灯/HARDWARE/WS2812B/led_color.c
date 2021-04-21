#include "led_color.h"
#define PI 3.14159*2
#define TASK_TIME 50


uint8_t dispaly_flag = 0;									//��ʾ��ɫģʽ��־λ
void dispaly_color_table(void);						//��ʾ������е���ɫ
void dispaly_color_seven(void);   				//��ʾ���ֵ�ɫ������ģʽ
void dispaly_star_mode(void);							//��ʾ����ģʽ
unsigned char temp_arr[LED_NUM_MAX*3]={0x00};	//����һ����������

 void all_off_2812(void)											//Ϩ�𲢸�λ2812���еĵ�
 {
   unsigned int j;
	 
	for(j = 0 ; j <= LED_NUM_MAX;j++)
	{	
		vWriteColor(0,0,0);
	}	
	vWriteRst(  ); 
 }

 
 void vWriteByte(uint8_t u8Data)
{
	uint8_t i;
	for(i = 0 ; i< 8 ;i++)
	{
	if(u8Data & 0x80)
		bsp_spiWrite1(0xFC);
	else
			bsp_spiWrite1(0xc0);
	
  u8Data <<= 1 ;
	
	}
}

void vWriteRst(void )
{
 unsigned int j;
	 
	for(j = 0 ; j <= LED_NUM_MAX;j++)
	{	
		bsp_spiWrite1(0x00);
   
	}	

}

void vWriteColor(uint8_t u8Red,uint8_t u8Green,uint8_t u8Blue)
{
		vWriteByte(u8Green);
		vWriteByte(u8Red );
		vWriteByte(u8Blue); 
}
//   
void  select_seven_color(uint8_t *var_RGB ,uint8_t luminance,uint8_t color_choose)
{
	switch (color_choose)
	{
		case 0 : var_RGB[0] = luminance; var_RGB[1] = 0; 				 var_RGB[2] = 0;  							break;	   //��
	  case 1 : var_RGB[0] = luminance; var_RGB[1] = luminance; var_RGB[2] = 0;								break;		 //��
		case 2 : var_RGB[0] = 0; 				 var_RGB[1] = luminance; var_RGB[2] = 0;								break;		 //��
		case 3 : var_RGB[0] = 0; 				 var_RGB[1] = luminance; var_RGB[2] = luminance;  			break;     //��
		case 4 : var_RGB[0] = 0; 				 var_RGB[1] = 0;				 var_RGB[2] = luminance;				break;	   //��
		case 5 : var_RGB[0] = luminance; var_RGB[1] = 0; 				 var_RGB[2] = luminance;				break;		 //��
		case 6 : var_RGB[0] = luminance; var_RGB[1] = luminance; var_RGB[2] = luminance;				break;		 //��	
		default: var_RGB[0] = 0; 				 var_RGB[1] = 0; 				 var_RGB[2] = 0;  							break;		 //��
	}
}
void SW2812_arr( uint8_t *arr )								//�����е�2812����д����
{
	uint8_t loop;
	for(loop=0;loop<LED_NUM_MAX;loop++)					
	{
		vWriteColor(arr[loop*3+0],arr[loop*3+1],arr[loop*3+2]);	//�������е�����д��2812��
	}
}


void WS2812_dispaly(void)
{


	switch (dispaly_flag)
	{
		case 0 :dispaly_color_seven();	 break;
		case 1 :dispaly_star_mode();	   break;
		default : all_off_2812();				 break;
	}
}

void dispaly_color_seven(void)   						//��ʾ���ֵ�ɫ������ģʽ
{
	uint8_t loop;
  uint8_t luminance_temp[3] = {0};					//���RGB��ɫֵ
	static int8_t luminance_color = 0;				//��ɫ������
  static uint8_t color_choose = RED;				//Ĭ����ʾ��ɫ
	static uint8_t limit_max_luminance = 128; //�޶�����Ϊ128  ��������Ϊ256
	static uint16_t priod = 15000;  						//��������(����)
	static uint32_t MoveTimeCnt = 0;					//��������ʱ���ʱ
	float Omega = 0.0;

	MoveTimeCnt += TASK_TIME;							 				//ÿ50ms����1��
	Omega = PI*(float)MoveTimeCnt / (float)priod;	//��2�н��й�һ������		
  
	for(loop=0;loop<LED_NUM_MAX;loop++)  					//�����黺����д�뽫Ҫ��ʾ������
	{	
		luminance_color = sin(Omega) * limit_max_luminance;
		if(luminance_color <0 )luminance_color = -luminance_color;      //�Ը�����ȡ����ֵ
		color_choose = (MoveTimeCnt*2/priod)%7;														//����������ں���һ����ɫ
		select_seven_color(luminance_temp,luminance_color,color_choose);//����Ҫ��ʾ����ɫ�����Ȳ�����ֵ���ظ�����������
		
	  temp_arr[loop*3+0] = luminance_temp[0];	
		temp_arr[loop*3+1] = luminance_temp[1];
		temp_arr[loop*3+2] = luminance_temp[2];
	}
	SW2812_arr( temp_arr );											//��ʾ���黺��������ɫ
	
	if(MoveTimeCnt/priod>=5)dispaly_flag++;     //������ɫȫ����ʾ���
}


void dispaly_color_table(void)
{
	uint8_t loop=0;
	static uint8_t luminance_grade = 2;  //  limit = 256/luminance_grade   1Ϊ���ֵ 
  static uint8_t color_order=0;
	static u32 last_time=0;
	if(xTaskGetTickCount() - last_time >=500)
	 {		
		for(loop=0;loop<LED_NUM_MAX;loop++)  //�����黺����д�뽫Ҫ��ʾ������
		{	
			temp_arr[loop*3+0] = color_arr[color_order*3+0]/luminance_grade;
			temp_arr[loop*3+1] = color_arr[color_order*3+1]/luminance_grade;
			temp_arr[loop*3+2] = color_arr[color_order*3+2]/luminance_grade;
		}
		SW2812_arr( temp_arr );							//��ʾ���黺��������ɫ
		if(color_order >=128)color_order = 0,dispaly_flag++;
			else	color_order++; 							//ָ����ɫ��������һ����ɫ
		 
		last_time = xTaskGetTickCount();
	 }

}


void dispaly_star_mode(void) 
{
	uint8_t loop;
  uint8_t luminance_temp[3] = {0};					//���RGB��ɫֵ
	uint8_t luminance_phase_1[7] = {0,PI*1/7,PI*2/7,PI*3/7,PI*4/7,PI*5/7,PI*6/7};//������ɫ������λ
	uint8_t luminance_phase_2[7] = {PI*4/7,PI*5/7,PI*6/7,0,PI*1/7,PI*2/7,PI*3/7};//��ͬ��λ ��ͬЧ��
	uint8_t luminance_phase_3[7] = {0,PI*4/7,PI*1/7,PI*5/7,PI*2/7,PI*6/7,PI*3/7};  
	static uint8_t phase_select = 0;
	static int8_t luminance_color = 0;				//��ɫ������
  static uint8_t color_choose = RED;				//Ĭ����ʾ��ɫ
	static uint8_t limit_max_luminance = 128; //�޶�����Ϊ128  ��������Ϊ256
	static uint16_t priod = 15000;  					//��������(����)
	static uint32_t MoveTimeCnt = 0;					//��������ʱ���ʱ
	float Omega = 0.0;

	MoveTimeCnt += TASK_TIME;							 				//ÿ50ms����1��
	Omega = PI*(float)MoveTimeCnt / (float)priod;	//��2�н��й�һ������		
	for(loop=0;loop<LED_NUM_MAX;loop++)  					//�����黺����д�뽫Ҫ��ʾ������
	{			
		if(phase_select == 0)luminance_color = sin(Omega + luminance_phase_1[loop%7]) * limit_max_luminance;       //��λѡ��
		  else if(phase_select == 1)luminance_color = sin(Omega + luminance_phase_2[loop%7]) * limit_max_luminance;
		  else luminance_color = sin(Omega + luminance_phase_3[loop%7]) * limit_max_luminance;
		
		if (luminance_color <0)luminance_color =0;
		color_choose = (loop%7 + (MoveTimeCnt/priod)%7)%7;		          //һ���������ں���һ����ɫ
		select_seven_color(luminance_temp,luminance_color,color_choose);//����Ҫ��ʾ����ɫ�����Ȳ�����ֵ���ظ�����������

	  temp_arr[loop*3+0] = luminance_temp[0];	
		temp_arr[loop*3+1] = luminance_temp[1];
		temp_arr[loop*3+2] = luminance_temp[2];
	}
	SW2812_arr( temp_arr );										 //��ʾ���黺��������ɫ
	if(MoveTimeCnt/priod > 4)									 //ÿ��һ���ӻ�һ����λ
	{
		MoveTimeCnt = 0;
		if(phase_select<3)phase_select++; 
		else phase_select = 0;
	}
}
void WS2812B_task(void *pvParameters)
{

  	TickType_t xLastWakeTime;  
		const TickType_t xFrequency = TASK_TIME;
		xLastWakeTime = xTaskGetTickCount();	
    while(1)
    {
 //     WS2812_dispaly();
//			dispaly_color_seven();
			dispaly_star_mode();
      vTaskDelayUntil(&xLastWakeTime, xFrequency);  
    } 
}
