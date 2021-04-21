#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 


#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
#endif


