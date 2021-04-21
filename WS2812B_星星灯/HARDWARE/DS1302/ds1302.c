#include "ds1302.h"

#define Change   1
#define Save     0
//READ_RTC ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d};//��ȡʱ��������ַ���Ѿ�ͨ����д������ֱ��ʵ����Щ��ַ
//WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};//дʱ��������ַ

/*1����ȡ��ʱ���û�а�DATA_IO�������͡�������ʾ�ʺź�85��һЩ���߰���Ķ�����
2����ѹ������С��4.6V������������������飬�ҽӵ���5V��ʵ��4.8V��Ӧ��û���⡣
3��û�н��������衣��ֻ����Ҫ˫��IO�ĵط������������裬���õ��ǰ�����Ԥ����IIC��SDA��������һ��4.7K���������Ұ�IO���������Ӧ��Ҳû����
4������ʱ�򲻶ԡ�������֮ǰ�����ʱ����51����ʵ�ֹ�һ���Ĺ��ܣ�������ֲ��32��Ӧ��Ҳûʲô���Ⱑ����ʱʱ��Ҳ�����ˣ��ϸ���1us����ʱ����ġ�
5��ȷʵҪע��DS1302�ĵ�ѹ����ò�Ҫ��STM32�����������3.3V����������û��������������ⲿ��Դ��DS1302����Ļ�����Ҫ���ⲿ��Դ�Ϳ����干�أ���Ȼ����ȫ��85.
6��DS1302�����Ҫ�޸�ʱ�䡣��Ҫ�ѳ�ʼ������������ϵ籣��ȥ�����ٴ��������õ�ʱ�䣬Ȼ���ٰ��ϵ籣���Ƕθ������ȥ����ֹ��λ��ʱ�䱻���á�*/


u8 init_time[] = {0x30,0x02,0x13,0x05,0x12,0x06,0x20}; //��ʼ��ʱ�䣺�� �� ʱ �� �� �� ��


static void ds1302_gpio_init(void);
static void ds1302_writebyte(u8 byte_1);//дһ���ֽ�; byte�Ǳ����֣�������Ϊ����
static void ds1302_writedata(u8 addr,u8 data_);//��ĳ��ַд����,data��c51�ڲ��Ĺؼ��֣���ʾ���������������ݴ洢�����ʴ˴���data_;
static u8 ds1302_readbyte(void);//��һ���ֽ�
static u8 ds1302_readdata(u8 addr);//��ȡĳ�Ĵ�������;
static void DS1302_delay_us(u16 time);  //����ʱ1us


//����IO����
static void ds1302_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;  
    
    //����GPIOD��ʱ��  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
    
    //����GPIO�Ļ�������  
    GPIO_InitStruct.GPIO_Pin = DS1302_SCK_PIN | DS1302_CE_PIN ;  
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    //��������ͨ�˿���Ϊ�������  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //����ٶ�50MHz  
    GPIO_Init(DS1302_PORT, &GPIO_InitStruct);  

    GPIO_InitStruct.GPIO_Pin = DS1302_IO_PIN;         //���������ɿ�©����ȻҲ������ͨ���죬������Ҫ����һֱ�л��������ģʽ
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;     //��©�������Ҫ������������Ҫ�л���������ˡ�
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //����ٶ�50MHz  
    GPIO_Init(DS1302_PORT, &GPIO_InitStruct);
}

//дһ���ֽ�
//���ݺ͵�ַ���Ǵ����λ��ʼ�����
static void ds1302_writebyte(u8 byte_1)
{
    u8 i = 0;
    u8 t = 0x01;
    
    for(i = 0;i<8;i++)
    {
        if((byte_1 & t) != 0)     //֮ǰ������������32��λ���������ܸ�ֵ0��1֮���ֵ��
        {
            DS1302_DATOUT = 1;
        }
        else
        {
            DS1302_DATOUT = 0;
        }
        
        DS1302_delay_us(2);
        DS1302_SCK = 1;  //������д��
        DS1302_delay_us(2);
        DS1302_SCK = 0; 
        DS1302_delay_us(2);
        
        t<<= 1;
    }
    DS1302_DATOUT = 1;      //�ͷ�IO�������ȡ�Ļ���׼ȷ�ܶ�
    DS1302_delay_us(2);     //��Ϊ���д��֮��IO�����˵͵�ƽ����©���ģʽ�¶�ȡ��ʱ�����Ӱ�죬��������ߣ��ٶ�ȡ
}

//��ַд����
static void ds1302_writedata(u8 addr,u8 data_)
{    
    DS1302_CE = 0;        DS1302_delay_us(2);    
    DS1302_SCK = 0;        DS1302_delay_us(2);    
    DS1302_CE = 1;        DS1302_delay_us(2);    //ʹ��Ƭѡ�ź�
    
    ds1302_writebyte((addr<<1)|0x80);    //�������д��,ת��֮���ǵ�ַ�Ĵ�����ֵ��
    ds1302_writebyte(data_);
    DS1302_CE = 0;        DS1302_delay_us(2);//�������ݽ�����ʧ��Ƭѡ
    DS1302_SCK = 0;     DS1302_delay_us(2);//���ͣ�׼����һ��д����
}

//��ȡһ���ֽ�,�����ض�ȡ
static u8 ds1302_readbyte(void)
{
    u8 i = 0;
    u8 data_ = 0;
    
    //��Ϊ�����Ѿ��Ѷ˿�����Ϊ��©����·�ⲿ�����������裬���Բ��л��������ģʽ��ֱ��ʹ�á�
    //    DS1302_DAT_INPUT();  
    
    DS1302_SCK = 0;
    DS1302_delay_us(3);
    for(i=0;i<7;i++)   //���﷢����Ϊ8�Ļ�������ݲ��ԣ�����
    {
        if((DS1302_DATIN) == 1) 
        {
            data_ = data_ | 0x80;    //��λ��ǰ����λ��ȡ
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

//��ȡ�Ĵ�����ֵ
static u8 ds1302_readdata(u8 addr)
{
    u8 data_ = 0;

    DS1302_CE = 0;        DS1302_delay_us(2);
    DS1302_SCK = 0;       DS1302_delay_us(2);
    DS1302_CE = 1;        DS1302_delay_us(2);   //��д����ʱCE����Ϊ�ߣ�����SCKΪ��ʱ�ı�
    
    ds1302_writebyte((addr<<1)|0x81);   //д���ʱ�������
    data_ = ds1302_readbyte(); 
     
    DS1302_SCK = 1;       DS1302_delay_us(2);
    DS1302_CE = 0;        DS1302_delay_us(2);
    DS1302_DATOUT = 0;    DS1302_delay_us(3);  //����ܶ���˵��Ҫ���ͣ������ҷ���ȥ�����Ҳ������ʾ��������Ϊ�˱��գ����Ǽ��ϡ�
    DS1302_DATOUT = 1;    DS1302_delay_us(2);

    return data_;
}

void DS1302_Init(void)
{
     u8 i = 0;
    
    ds1302_gpio_init();  //�˿ڳ�ʼ��
    
    DS1302_CE = 0;  DS1302_delay_us(2);
    DS1302_SCK = 0; DS1302_delay_us(2);  
    
    i  = ds1302_readdata(0x00);  //��ȡ��Ĵ���,

     if((i & 0x80) != Save)//ͨ���ж���Ĵ����Ƿ��������������´��ϵ��ʱ���Ƿ��ʼ��ʱ�䣬���ǵ��籣��
    {
         ds1302_writedata(7,0x01); //����д����������д������,0x8e,0x00

        for(i = 0;i<7;i++)
        {
            ds1302_writedata(i,init_time[i]);
        }
    }
         ds1302_writedata(7,0x80);//��д�������ܣ���ֹ������ɵ�����д�롣
}


void Ds1302_readtime(void)   //��ȡʱ��
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
      i = 5;  //�Լ�����
      while(i--);
   }
}

