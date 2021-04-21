#ifndef __LED_COLOR_H
#define __LED_COLOR_H
#include "config.h"

#define LED_NUM_MAX 60

#define RED    0			//��ɫ
#define ORANGE 1			//��ɫ
#define GREEN  2			//��ɫ
#define CYAN   3			//��ɫ
#define BLUE   4			//��ɫ
#define PURPLE 5			//��ɫ
#define WHITE  6			//��ɫ
void all_off_2812(void);
void vWriteByte(unsigned char u8Data) ;
void vWriteRst(void );
void vWriteColor(uint8_t u8Red,uint8_t u8Green,uint8_t u8Blue);
void seven_color(uint8_t color_choose ,uint8_t luminance);
char one_onled(void);

void WS2812_dispaly(void);
void WS2812B_task(void *pvParameters);
#endif

