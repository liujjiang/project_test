#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 


#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
#endif


