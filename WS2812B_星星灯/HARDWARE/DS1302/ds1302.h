#ifndef __DS1302_H
#define __DS1302_H

#include "config.h"

//相对应的IO口配置
#define DS1302_PORT           GPIOB

#define DS1302_SCK_PIN        GPIO_Pin_14        //时钟
#define DS1302_IO_PIN         GPIO_Pin_15    //双向IO口
#define DS1302_CE_PIN         GPIO_Pin_13   //片选使能，当需要读写的时候，置高位

#define DS1302_SCK            PBout(14)  //位带操作，可直接给高低电平，但是切记不能给0.1之外的数
#define DS1302_CE             PBout(13)
#define DS1302_DATIN          PBin(15)  
#define DS1302_DATOUT         PBout(15)
//存放时间
typedef struct _time{ 

    u8 second;
    u8 minute;
    u8 hour;
    u8 date;
    u8 month;
    u8 week;
    u8 year;

}my_time;

void DS1302_Init(void);
void Ds1302_readtime(void);
//void display_real_time(void);  //显示实时时间

#endif

