#ifndef __LED_COLOR_H
#define __LED_COLOR_H
#include "config.h"

#define LED_NUM_MAX 60

#define RED    0			//红色
#define ORANGE 1			//橙色
#define GREEN  2			//绿色
#define CYAN   3			//青色
#define BLUE   4			//蓝色
#define PURPLE 5			//紫色
#define WHITE  6			//白色
void all_off_2812(void);
void vWriteByte(unsigned char u8Data) ;
void vWriteRst(void );
void vWriteColor(uint8_t u8Red,uint8_t u8Green,uint8_t u8Blue);
void seven_color(uint8_t color_choose ,uint8_t luminance);
char one_onled(void);

void WS2812_dispaly(void);
void WS2812B_task(void *pvParameters);
#endif

