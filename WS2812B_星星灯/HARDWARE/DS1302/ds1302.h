#ifndef __DS1302_H
#define __DS1302_H

#include "config.h"

//���Ӧ��IO������
#define DS1302_PORT           GPIOB

#define DS1302_SCK_PIN        GPIO_Pin_14        //ʱ��
#define DS1302_IO_PIN         GPIO_Pin_15    //˫��IO��
#define DS1302_CE_PIN         GPIO_Pin_13   //Ƭѡʹ�ܣ�����Ҫ��д��ʱ���ø�λ

#define DS1302_SCK            PBout(14)  //λ����������ֱ�Ӹ��ߵ͵�ƽ�������мǲ��ܸ�0.1֮�����
#define DS1302_CE             PBout(13)
#define DS1302_DATIN          PBin(15)  
#define DS1302_DATOUT         PBout(15)
//���ʱ��
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
//void display_real_time(void);  //��ʾʵʱʱ��

#endif

