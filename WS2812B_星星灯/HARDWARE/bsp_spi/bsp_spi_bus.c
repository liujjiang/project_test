#include "bsp_spi_bus.h"
/*
	PA5/SPI1_SCK                 PB13/SPI1_SCK
	PA6/SPI1_MISO                PB14/SPI1_MISO
	PA7/SPI1_MOSI                PB15/SPI1_MOSI
	SPI1的时钟源是 APB2Periph    SPI2的时钟源是 APB1Periph
*/
//#define SPI_1
#define SPI_2
/* 定义SPI总线的 GPIO端口 */
#if SPI_1
#define SPI_HARD	    SPI1
#define RCC_SPI_PIN 	RCC_APB2Periph_GPIOA
#define RCC_SPI		    RCC_APB2Periph_SPI1
#define PORT_SPI     	GPIOA
#define PIN_SCK		    GPIO_Pin_5
#define PIN_MISO	    GPIO_Pin_6
#define PIN_MOSI	    GPIO_Pin_7
#else
#define SPI_HARD	   SPI2
#define RCC_SPI		   RCC_APB1Periph_SPI2
#define RCC_SPI_PIN  RCC_APB2Periph_GPIOB
#define PORT_SPI	   GPIOB
#define PIN_SCK		   GPIO_Pin_13
#define PIN_MISO	   GPIO_Pin_14
#define PIN_MOSI	   GPIO_Pin_15
#endif
uint8_t g_spi_busy = 0;		/* SPI 总线共享标志 */
/*
*********************************************************************************************************
*	函 数 名: bsp_InitSPIBus
*	功能说明: 配置SPI总线。 只包括 SCK、 MOSI、 MISO口线的配置。不包括片选CS，也不包括外设芯片特有的INT、BUSY等
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSPIBus(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	/* 开启 SPI 时钟 */
#if SPI_1	
	RCC_APB2PeriphClockCmd(RCC_SPI, ENABLE);	
#else	
	RCC_APB1PeriphClockCmd(RCC_SPI, ENABLE);	
#endif
//	RCC_APB2PeriphClockCmd(RCC_SPI, ENABLE);	
	/* 使能 GPIO 时钟 */
	RCC_APB2PeriphClockCmd(RCC_SPI_PIN, ENABLE);	
	/* 配置 SPI引脚SCK、MISO 和 MOSI为复用推挽模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = PIN_SCK | PIN_MISO | PIN_MOSI;	
	GPIO_Init(PORT_SPI, &GPIO_InitStructure);
	
		/* 配置SPI硬件参数 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* 数据方向：2线全双工 */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		/* STM32的SPI工作模式 ：主机模式 */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* 数据位长度 ： 8位 */
	/* SPI_CPOL和SPI_CPHA结合使用决定时钟和数据采样点的相位关系、
	   本例配置: 总线空闲是高电平,第2个边沿（上升沿采样数据)      */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;			/* 时钟上升沿采样数据 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		/* 时钟的第2个边沿采样数据 */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			/* 片选控制方式：软件控制 */
	/* 设置波特率预分频系数 SPI_BaudRatePrescaler_8 ，实测SCK周期 96ns, 10.4MHz */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* 数据位传输次序：高位先传 */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC多项式寄存器，复位后为7。本例程不用 */
	SPI_Init(SPI_HARD, &SPI_InitStructure);
	
	/* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
	SPI_HARD->I2SCFGR &= SPI_Mode_Select;		/* 选择SPI模式，不是I2S模式 */
	/*---------------------------- SPIx CRCPOLY Configuration --------------------*/
	/* Write to SPIx CRCPOLY */
	SPI_HARD->CRCPR = 7;		/* 一般不用 */

	SPI_Cmd(SPI_HARD, DISABLE);			/* 先禁止SPI  */
	SPI_Cmd(SPI_HARD, ENABLE);			/* 使能SPI  */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SPI_Init
*	功能说明: 配置STM32内部SPI硬件的工作模式。 简化库函数，提高执行效率。 仅用于SPI接口间切换。
*	形    参: _cr1 寄存器值
*	返 回 值: 无
*********************************************************************************************************
*/

void bsp_SPI_Init(uint16_t _cr1)
{
	SPI_HARD->CR1 = ((SPI_HARD->CR1 & CR1_CLEAR_Mask) | _cr1);
	  
	//SPI_Cmd(SPI_HARD, DISABLE);			/* 先禁止SPI  */	    
    SPI_HARD->CR1 &= CR1_SPE_Reset;	/* Disable the selected SPI peripheral */

	//SPI_Cmd(SPI_HARD, ENABLE);			/* 使能SPI  */		    
    SPI_HARD->CR1 |= CR1_SPE_Set;	  /* Enable the selected SPI peripheral */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_spiWrite0
*	功能说明: 向SPI总线发送一个字节。SCK上升沿采集数据, SCK空闲时为低电平。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_spiWrite0(uint8_t _ucByte)
{
	/* 等待发送缓冲区空 */
	while(SPI_I2S_GetFlagStatus(SPI_HARD, SPI_I2S_FLAG_TXE) == RESET);

	/* 发送一个字节 */
	SPI_I2S_SendData(SPI_HARD, _ucByte);

	/* 等待数据接收完毕 */
	while(SPI_I2S_GetFlagStatus(SPI_HARD, SPI_I2S_FLAG_RXNE) == RESET);

	/* 读取接收到的数据 */
	SPI_I2S_ReceiveData(SPI_HARD);

}

/*
*********************************************************************************************************
*	函 数 名: bsp_spiRead0
*	功能说明: 从SPI总线接收8个bit数据。 SCK上升沿采集数据, SCK空闲时为低电平。
*	形    参: 无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t bsp_spiRead0(void)
{
	uint8_t read;

	/* 等待发送缓冲区空 */
	while(SPI_I2S_GetFlagStatus(SPI_HARD, SPI_I2S_FLAG_TXE) == RESET);

	/* 发送一个字节 */
	SPI_I2S_SendData(SPI_HARD, 0);

	/* 等待数据接收完毕 */
	while(SPI_I2S_GetFlagStatus(SPI_HARD, SPI_I2S_FLAG_RXNE) == RESET);

	/* 读取接收到的数据 */
	read = SPI_I2S_ReceiveData(SPI_HARD);

	/* 返回读到的数据 */
	return read;

}

/*
*********************************************************************************************************
*	函 数 名: bsp_spiWrite1
*	功能说明: 向SPI总线发送一个字节。  SCK上升沿采集数据, SCK空闲时为高电平
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_spiWrite1(uint8_t _ucByte)
{
	/* 等待发送缓冲区空 */
	while(SPI_I2S_GetFlagStatus(SPI_HARD, SPI_I2S_FLAG_TXE) == RESET);

	/* 发送一个字节 */
	SPI_I2S_SendData(SPI_HARD, _ucByte);

	/* 等待数据接收完毕 */
	while(SPI_I2S_GetFlagStatus(SPI_HARD, SPI_I2S_FLAG_RXNE) == RESET);

	/* 读取接收到的数据 */
	SPI_I2S_ReceiveData(SPI_HARD);

}

/*
*********************************************************************************************************
*	函 数 名: bsp_spiRead1
*	功能说明: 从SPI总线接收8个bit数据。  SCK上升沿采集数据, SCK空闲时为高电平
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bsp_spiRead1(void)
{
	uint8_t read;

	/* 等待发送缓冲区空 */
	while(SPI_I2S_GetFlagStatus(SPI_HARD, SPI_I2S_FLAG_TXE) == RESET);

	/* 发送一个字节 */
	SPI_I2S_SendData(SPI_HARD, 0);

	/* 等待数据接收完毕 */
	while(SPI_I2S_GetFlagStatus(SPI_HARD, SPI_I2S_FLAG_RXNE) == RESET);

	/* 读取接收到的数据 */
	read = SPI_I2S_ReceiveData(SPI_HARD);

	/* 返回读到的数据 */
	return read;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusEnter
*	功能说明: 占用SPI总线
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
void bsp_SpiBusEnter(void)
{
	g_spi_busy = 1;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusExit
*	功能说明: 释放占用的SPI总线
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
void bsp_SpiBusExit(void)
{
	g_spi_busy = 0;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusBusy
*	功能说明: 判断SPI总线忙。方法是检测其他SPI芯片的片选信号是否为1
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
uint8_t bsp_SpiBusBusy(void)
{
	return g_spi_busy;
}



/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
