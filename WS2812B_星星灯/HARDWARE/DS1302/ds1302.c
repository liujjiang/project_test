#include "ds1302.h"

#define Change   1
#define Save     0
//READ_RTC ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d};//读取时间的命令地址，已经通过读写操作来直接实现这些地址
//WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};//写时间的命令地址

/*1、读取完时间后没有把DATA_IO引脚拉低。导致显示问号和85等一些乱七八糟的东西。
2、电压不够，小于4.6V。但是这个网上有争议，我接的是5V，实测4.8V，应该没问题。
3、没有接上拉电阻。我只在需要双向IO的地方加了上拉电阻，利用的是板子上预留的IIC的SDA，上面有一个4.7K的上拉，我把IO接在了这里。应该也没问题
4、仿真时序不对。但是我之前用这个时序在51上面实现过一样的功能，现在移植到32上应该也没什么问题啊，延时时间也仿真了，严格按照1us的延时仿真的。
5、确实要注意DS1302的电压，最好不要用STM32开发板上面的3.3V，反正我是没做出来。如果用外部电源给DS1302供电的话，需要将外部电源和开发板共地，不然读出全是85.
6、DS1302如果需要修改时间。需要把初始化函数里面的上电保护去掉，再次下载重置的时间，然后再把上电保护那段给添加上去，防止复位后时间被重置。*/


u8 init_time[] = {0x30,0x02,0x13,0x05,0x12,0x06,0x20}; //初始化时间：秒 分 时 日 月 周 年


static void ds1302_gpio_init(void);
static void ds1302_writebyte(u8 byte_1);//写一个字节; byte是保留字，不能作为变量
static void ds1302_writedata(u8 addr,u8 data_);//给某地址写数据,data是c51内部的关键字，表示将变量定义在数据存储区，故此处用data_;
static u8 ds1302_readbyte(void);//读一个字节
static u8 ds1302_readdata(u8 addr);//读取某寄存器数据;
static void DS1302_delay_us(u16 time);  //简单延时1us


//基本IO设置
static void ds1302_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;  
    
    //开启GPIOD的时钟  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
    
    //设置GPIO的基本参数  
    GPIO_InitStruct.GPIO_Pin = DS1302_SCK_PIN | DS1302_CE_PIN ;  
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    //这两个普通端口设为推挽输出  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //输出速度50MHz  
    GPIO_Init(DS1302_PORT, &GPIO_InitStruct);  

    GPIO_InitStruct.GPIO_Pin = DS1302_IO_PIN;         //这里最好设成开漏，当然也可以普通推挽，但是需要后面一直切换输入输出模式
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;     //开漏输出，需要接上拉，不需要切换输入输出了。
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //输出速度50MHz  
    GPIO_Init(DS1302_PORT, &GPIO_InitStruct);
}

//写一个字节
//数据和地址都是从最低位开始传输的
static void ds1302_writebyte(u8 byte_1)
{
    u8 i = 0;
    u8 t = 0x01;
    
    for(i = 0;i<8;i++)
    {
        if((byte_1 & t) != 0)     //之前的问题出在这里，32的位带操作不能赋值0和1之外的值。
        {
            DS1302_DATOUT = 1;
        }
        else
        {
            DS1302_DATOUT = 0;
        }
        
        DS1302_delay_us(2);
        DS1302_SCK = 1;  //上升沿写入
        DS1302_delay_us(2);
        DS1302_SCK = 0; 
        DS1302_delay_us(2);
        
        t<<= 1;
    }
    DS1302_DATOUT = 1;      //释放IO，后面读取的话会准确很多
    DS1302_delay_us(2);     //因为如果写完之后IO被置了低电平，开漏输出模式下读取的时候会有影响，最好先拉高，再读取
}

//地址写数据
static void ds1302_writedata(u8 addr,u8 data_)
{    
    DS1302_CE = 0;        DS1302_delay_us(2);    
    DS1302_SCK = 0;        DS1302_delay_us(2);    
    DS1302_CE = 1;        DS1302_delay_us(2);    //使能片选信号
    
    ds1302_writebyte((addr<<1)|0x80);    //方便后面写入,转化之后是地址寄存器的值，
    ds1302_writebyte(data_);
    DS1302_CE = 0;        DS1302_delay_us(2);//传送数据结束，失能片选
    DS1302_SCK = 0;     DS1302_delay_us(2);//拉低，准备下一次写数据
}

//读取一个字节,上升沿读取
static u8 ds1302_readbyte(void)
{
    u8 i = 0;
    u8 data_ = 0;
    
    //因为上面已经把端口设置为开漏，电路外部接了上拉电阻，可以不切换输入输出模式，直接使用。
    //    DS1302_DAT_INPUT();  
    
    DS1302_SCK = 0;
    DS1302_delay_us(3);
    for(i=0;i<7;i++)   //这里发现设为8的话输出数据不对，很乱
    {
        if((DS1302_DATIN) == 1) 
        {
            data_ = data_ | 0x80;    //低位在前，逐位读取
        }
        data_>>= 1;
        DS1302_delay_us(3);
        
        DS1302_SCK = 1;
        DS1302_delay_us(3);
        DS1302_SCK = 0;
        DS1302_delay_us(3);
    }
     return (data_);
}

//读取寄存器的值
static u8 ds1302_readdata(u8 addr)
{
    u8 data_ = 0;

    DS1302_CE = 0;        DS1302_delay_us(2);
    DS1302_SCK = 0;       DS1302_delay_us(2);
    DS1302_CE = 1;        DS1302_delay_us(2);   //读写操作时CE必须为高，切在SCK为低时改变
    
    ds1302_writebyte((addr<<1)|0x81);   //写入读时间的命令
    data_ = ds1302_readbyte(); 
     
    DS1302_SCK = 1;       DS1302_delay_us(2);
    DS1302_CE = 0;        DS1302_delay_us(2);
    DS1302_DATOUT = 0;    DS1302_delay_us(3);  //这里很多人说需要拉低，但是我发现去掉这个也可以显示啊，不过为了保险，还是加上。
    DS1302_DATOUT = 1;    DS1302_delay_us(2);

    return data_;
}

void DS1302_Init(void)
{
     u8 i = 0;
    
    ds1302_gpio_init();  //端口初始化
    
    DS1302_CE = 0;  DS1302_delay_us(2);
    DS1302_SCK = 0; DS1302_delay_us(2);  
    
    i  = ds1302_readdata(0x00);  //读取秒寄存器,

     if((i & 0x80) != Save)//通过判断秒寄存器是否还有数据来决定下次上电的时候是否初始化时间，就是掉电保护
    {
         ds1302_writedata(7,0x01); //撤销写保护，允许写入数据,0x8e,0x00

        for(i = 0;i<7;i++)
        {
            ds1302_writedata(i,init_time[i]);
        }
    }
         ds1302_writedata(7,0x80);//打开写保护功能，防止干扰造成的数据写入。
}


void Ds1302_readtime(void)   //读取时间
{
      u8 i;
      for(i = 0;i<7;i++)
      {
         init_time[i] = ds1302_readdata(i);
      }
}

static void DS1302_delay_us(u16 time)
{    
   u16 i = 0;  
   while(time--)
   {
      i = 5;  //自己定义
      while(i--);
   }
}

