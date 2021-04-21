#include "led_color.h"
#define PI 3.14159*2
#define TASK_TIME 50


uint8_t dispaly_flag = 0;									//显示颜色模式标志位
void dispaly_color_table(void);						//显示数组表中的颜色
void dispaly_color_seven(void);   				//显示七种单色呼吸灯模式
void dispaly_star_mode(void);							//显示星星模式
unsigned char temp_arr[LED_NUM_MAX*3]={0x00};	//定义一个缓冲数组

 void all_off_2812(void)											//熄灭并复位2812所有的灯
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
		case 0 : var_RGB[0] = luminance; var_RGB[1] = 0; 				 var_RGB[2] = 0;  							break;	   //红
	  case 1 : var_RGB[0] = luminance; var_RGB[1] = luminance; var_RGB[2] = 0;								break;		 //橙
		case 2 : var_RGB[0] = 0; 				 var_RGB[1] = luminance; var_RGB[2] = 0;								break;		 //绿
		case 3 : var_RGB[0] = 0; 				 var_RGB[1] = luminance; var_RGB[2] = luminance;  			break;     //青
		case 4 : var_RGB[0] = 0; 				 var_RGB[1] = 0;				 var_RGB[2] = luminance;				break;	   //蓝
		case 5 : var_RGB[0] = luminance; var_RGB[1] = 0; 				 var_RGB[2] = luminance;				break;		 //紫
		case 6 : var_RGB[0] = luminance; var_RGB[1] = luminance; var_RGB[2] = luminance;				break;		 //白	
		default: var_RGB[0] = 0; 				 var_RGB[1] = 0; 				 var_RGB[2] = 0;  							break;		 //黑
	}
}
void SW2812_arr( uint8_t *arr )								//向所有的2812灯中写数据
{
	uint8_t loop;
	for(loop=0;loop<LED_NUM_MAX;loop++)					
	{
		vWriteColor(arr[loop*3+0],arr[loop*3+1],arr[loop*3+2]);	//将数组中的数据写入2812中
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

void dispaly_color_seven(void)   						//显示七种单色呼吸灯模式
{
	uint8_t loop;
  uint8_t luminance_temp[3] = {0};					//存放RGB的色值
	static int8_t luminance_color = 0;				//颜色的亮度
  static uint8_t color_choose = RED;				//默认显示红色
	static uint8_t limit_max_luminance = 128; //限度上线为128  最大的亮度为256
	static uint16_t priod = 15000;  						//亮灭周期(毫秒)
	static uint32_t MoveTimeCnt = 0;					//任务运行时间计时
	float Omega = 0.0;

	MoveTimeCnt += TASK_TIME;							 				//每50ms运算1次
	Omega = PI*(float)MoveTimeCnt / (float)priod;	//对2π进行归一化处理		
  
	for(loop=0;loop<LED_NUM_MAX;loop++)  					//向数组缓冲区写入将要显示的数据
	{	
		luminance_color = sin(Omega) * limit_max_luminance;
		if(luminance_color <0 )luminance_color = -luminance_color;      //对负周期取绝对值
		color_choose = (MoveTimeCnt*2/priod)%7;														//半个亮灭周期后换下一种颜色
		select_seven_color(luminance_temp,luminance_color,color_choose);//调整要显示的颜色和亮度并将数值返回给缓冲数组中
		
	  temp_arr[loop*3+0] = luminance_temp[0];	
		temp_arr[loop*3+1] = luminance_temp[1];
		temp_arr[loop*3+2] = luminance_temp[2];
	}
	SW2812_arr( temp_arr );											//显示数组缓冲区的颜色
	
	if(MoveTimeCnt/priod>=5)dispaly_flag++;     //其中颜色全部显示完毕
}


void dispaly_color_table(void)
{
	uint8_t loop=0;
	static uint8_t luminance_grade = 2;  //  limit = 256/luminance_grade   1为最大值 
  static uint8_t color_order=0;
	static u32 last_time=0;
	if(xTaskGetTickCount() - last_time >=500)
	 {		
		for(loop=0;loop<LED_NUM_MAX;loop++)  //向数组缓冲区写入将要显示的数据
		{	
			temp_arr[loop*3+0] = color_arr[color_order*3+0]/luminance_grade;
			temp_arr[loop*3+1] = color_arr[color_order*3+1]/luminance_grade;
			temp_arr[loop*3+2] = color_arr[color_order*3+2]/luminance_grade;
		}
		SW2812_arr( temp_arr );							//显示数组缓冲区的颜色
		if(color_order >=128)color_order = 0,dispaly_flag++;
			else	color_order++; 							//指向颜色代码表的下一个颜色
		 
		last_time = xTaskGetTickCount();
	 }

}


void dispaly_star_mode(void) 
{
	uint8_t loop;
  uint8_t luminance_temp[3] = {0};					//存放RGB的色值
	uint8_t luminance_phase_1[7] = {0,PI*1/7,PI*2/7,PI*3/7,PI*4/7,PI*5/7,PI*6/7};//给灯颜色赋初相位
	uint8_t luminance_phase_2[7] = {PI*4/7,PI*5/7,PI*6/7,0,PI*1/7,PI*2/7,PI*3/7};//不同相位 不同效果
	uint8_t luminance_phase_3[7] = {0,PI*4/7,PI*1/7,PI*5/7,PI*2/7,PI*6/7,PI*3/7};  
	static uint8_t phase_select = 0;
	static int8_t luminance_color = 0;				//颜色的亮度
  static uint8_t color_choose = RED;				//默认显示红色
	static uint8_t limit_max_luminance = 128; //限度上线为128  最大的亮度为256
	static uint16_t priod = 15000;  					//亮灭周期(毫秒)
	static uint32_t MoveTimeCnt = 0;					//任务运行时间计时
	float Omega = 0.0;

	MoveTimeCnt += TASK_TIME;							 				//每50ms运算1次
	Omega = PI*(float)MoveTimeCnt / (float)priod;	//对2π进行归一化处理		
	for(loop=0;loop<LED_NUM_MAX;loop++)  					//向数组缓冲区写入将要显示的数据
	{			
		if(phase_select == 0)luminance_color = sin(Omega + luminance_phase_1[loop%7]) * limit_max_luminance;       //相位选择
		  else if(phase_select == 1)luminance_color = sin(Omega + luminance_phase_2[loop%7]) * limit_max_luminance;
		  else luminance_color = sin(Omega + luminance_phase_3[loop%7]) * limit_max_luminance;
		
		if (luminance_color <0)luminance_color =0;
		color_choose = (loop%7 + (MoveTimeCnt/priod)%7)%7;		          //一个亮灭周期后换下一种颜色
		select_seven_color(luminance_temp,luminance_color,color_choose);//调整要显示的颜色和亮度并将数值返回给缓冲数组中

	  temp_arr[loop*3+0] = luminance_temp[0];	
		temp_arr[loop*3+1] = luminance_temp[1];
		temp_arr[loop*3+2] = luminance_temp[2];
	}
	SW2812_arr( temp_arr );										 //显示数组缓冲区的颜色
	if(MoveTimeCnt/priod > 4)									 //每个一分钟换一次相位
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
