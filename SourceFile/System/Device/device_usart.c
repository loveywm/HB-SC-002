/*
********************************************************************************
*                                嵌入式微系统
*                                   msOS
*
********************************************************************************
*文件名     : device_usart.c
*作用       : 串口设备
*原理       : 发送采用Buffer发送，接收采用App注册函数方式接收
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
*V0.2    Wangsw        2013/09/13       第一次更改
********************************************************************************
*/

#include "drive.h"
#include "system.h"

typedef void (*RxdRegisterFunction)(byte data);

typedef struct 
{
    byte    * Start;    //指向队列开始 
    byte    * End;      //指向队列结束 
    byte    * In;       //插入一个消息  
    byte    * Out;      //取出一个消息  
    ushort    Size;     // 队列长度 
    ushort    Entries;  //消息长度      
}DataQueue;

typedef struct
{
    DataQueue Txd;
//    DataQueue Rxd;
}UsartDataQueue;

#define Usart1BufferSum 1000
static byte Usart1TxdBuffer[Usart1BufferSum];
//static byte Usart1RxdBuffer[Usart1BufferSum];

#define Usart2BufferSum 1000
static byte Usart2TxdBuffer[Usart2BufferSum];

#define Usart3BufferSum 1000
static byte Usart3TxdBuffer[Usart3BufferSum];



static void Dummy(byte data) {};

static UsartDataQueue Usart1DataQueue;
static UsartDataQueue Usart2DataQueue;
static UsartDataQueue Usart3DataQueue;

static RxdRegisterFunction Usart1RxdRegisterFunction = Dummy;
static RxdRegisterFunction Usart2RxdRegisterFunction = Dummy;
static RxdRegisterFunction Usart3RxdRegisterFunction = Dummy;

static bool WriteByteToUsart1(byte data);
static bool WriteByteToUsart2(byte data);
static bool WriteByteToUsart3(byte data);


/*******************************************************************************
* 函数名	: fputc
* 描述	    : Keil编译器支持printf函数，需要调用此函数，注意这个需要MicorLib支持
* 输入参数  : ch 实为byte类型，实际发送的数据
*           : f 设备，这儿不需要处理
* 返回参数  : 
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/10       初始版本
*******************************************************************************/
int fputc(int ch, FILE *f)
{
    if (ch == '\n')
    {
        while(!WriteByteToUsart1('\r'));
    }

    while(!WriteByteToUsart1((byte)ch));

    return ch;
}

/*******************************************************************************
* 函数名	: USART1_IRQHandler
* 描述	    : 默认串口接收中断入口，接收采用App上层注册函数机制，在App中运行处理
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/10       初始版本
*******************************************************************************/
void USART1_IRQHandler(void) 
{
    byte data;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    {
        data = (byte)USART_ReceiveData(USART1);
        
        Usart1RxdRegisterFunction(data);
    } 
} 
void USART2_IRQHandler(void) 
{
    byte data;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
    {
        data = (byte)USART_ReceiveData(USART2);
        
        Usart2RxdRegisterFunction(data);
    } 
} 
void USART3_IRQHandler(void) 
{
    byte data;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) 
    {
        data = (byte)USART_ReceiveData(USART3);
        
        Usart3RxdRegisterFunction(data);
    } 
} 

/*******************************************************************************
* 函数名	: UsartSystick1000Routine
* 描述	    : 系统节拍1000/S,即1mS一次调用此服务程序发送缓冲区数据
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/10       初始版本
*******************************************************************************/
void UsartSystick1000Routine(void) 
{
    if (Usart1DataQueue.Txd.Entries == 0) return;

    if ((USART1->SR&0X40)==0) return;

    USART1->DR = *Usart1DataQueue.Txd.Out++;
    Usart1DataQueue.Txd.Entries--;
    
    if (Usart1DataQueue.Txd.Out == Usart1DataQueue.Txd.End) 
    {
        Usart1DataQueue.Txd.Out = Usart1DataQueue.Txd.Start;
    }
}
void Usart2Systick1000Routine(void) 
{
    if (Usart2DataQueue.Txd.Entries == 0) return;

    if ((USART2->SR&0X40)==0) return;

    USART2->DR = *Usart2DataQueue.Txd.Out++;
    Usart2DataQueue.Txd.Entries--;
    
    if (Usart2DataQueue.Txd.Out == Usart2DataQueue.Txd.End) 
    {
        Usart2DataQueue.Txd.Out = Usart2DataQueue.Txd.Start;
    }
}

void Usart3Systick1000Routine(void) 
{
    if (Usart3DataQueue.Txd.Entries == 0) return;

    if ((USART3->SR&0X40)==0) return;

    USART3->DR = *Usart3DataQueue.Txd.Out++;
    Usart3DataQueue.Txd.Entries--;
    
    if (Usart3DataQueue.Txd.Out == Usart3DataQueue.Txd.End) 
    {
        Usart3DataQueue.Txd.Out = Usart3DataQueue.Txd.Start;
    }
}


static void OpenUsart1(void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}
static void OpenUsart2(void)
{
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
}
static void OpenUsart3(void)
{
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);
}

static void CloseUsart1(void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_Cmd(USART1, DISABLE);
}
static void CloseUsart2(void)
{
    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    USART_Cmd(USART2, DISABLE);
}
static void CloseUsart3(void)
{
    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
    USART_Cmd(USART3, DISABLE);
}


/*******************************************************************************
* 函数名	: Usart1RxdRegister
* 描述	    : Usart1接收注册函数，被App层调用注册需要回调处理Usart1接收的函数
* 输入参数  : registerFunction App层接收处理函数
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/14       初始版本
*******************************************************************************/
static void Usart1RxdRegister(void * registerFunction)
{
    Usart1RxdRegisterFunction = (RxdRegisterFunction)registerFunction;
}
static void Usart2RxdRegister(void * registerFunction)
{
    Usart2RxdRegisterFunction = (RxdRegisterFunction)registerFunction;
}
static void Usart3RxdRegister(void * registerFunction)
{
    Usart3RxdRegisterFunction = (RxdRegisterFunction)registerFunction;
}

/*******************************************************************************
* 函数名	: WriteByteToUsart1
* 描述	    : 串口1发送一个字节
* 输入参数  : data 字节类型，发送参数
* 返回参数  : bool 布尔类型，返回是否成功
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/13       初始版本
*******************************************************************************/
static bool WriteByteToUsart1(byte data) 
{
    if (Usart1DataQueue.Txd.Entries == Usart1BufferSum) return (false);
    
    EnterCritical();

    *Usart1DataQueue.Txd.In++ = data;	
    Usart1DataQueue.Txd.Entries++;
	
    if (Usart1DataQueue.Txd.In == Usart1DataQueue.Txd.End) 
        Usart1DataQueue.Txd.In = Usart1DataQueue.Txd.Start;
    
    ExitCritical();
    return (true);
}
static bool WriteByteToUsart2(byte data) 
{
    if (Usart2DataQueue.Txd.Entries == Usart2BufferSum) return (false);
    
    EnterCritical();

    *Usart2DataQueue.Txd.In++ = data;	
    Usart2DataQueue.Txd.Entries++;
	
    if (Usart2DataQueue.Txd.In == Usart2DataQueue.Txd.End) 
        Usart2DataQueue.Txd.In = Usart2DataQueue.Txd.Start;
    
    ExitCritical();
    return (true);
}

static bool WriteByteToUsart3(byte data) 
{
    if (Usart3DataQueue.Txd.Entries == Usart3BufferSum) return (false);
    
    EnterCritical();

    *Usart3DataQueue.Txd.In++ = data;	
    Usart3DataQueue.Txd.Entries++;
	
    if (Usart3DataQueue.Txd.In == Usart3DataQueue.Txd.End) 
        Usart3DataQueue.Txd.In = Usart3DataQueue.Txd.Start;
    
    ExitCritical();
    return (true);
}


/*******************************************************************************
* 函数名	: WriteToUsart1
* 描述	    : 串口1发送一串数据
* 输入参数  : dataPointer 数据指针，sum 数据总数
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/13       初始版本
*******************************************************************************/
static void WriteToUsart1(byte * dataPointer, int sum)
{
    if (sum)
    {
        while (sum--) 
        {
            WriteByteToUsart1(*dataPointer++);
        }
    }
    else
    {
        while (*dataPointer) 
        {
            if (*dataPointer == '\n') 
            {
                WriteByteToUsart1('\r');
            }
            WriteByteToUsart1(*dataPointer++);
        }
    }
}

static void InitializeUsartQueue(void)
{
    Usart1DataQueue.Txd.Start = Usart1TxdBuffer;
    Usart1DataQueue.Txd.End = Usart1TxdBuffer + Usart1BufferSum;
    Usart1DataQueue.Txd.In = Usart1DataQueue.Txd.Start;
    Usart1DataQueue.Txd.Out = Usart1DataQueue.Txd.Start;
    Usart1DataQueue.Txd.Size = Usart1BufferSum;
    Usart1DataQueue.Txd.Entries = 0;
}
static void InitializeUsart2Queue(void)
{
    Usart2DataQueue.Txd.Start = Usart2TxdBuffer;
    Usart2DataQueue.Txd.End = Usart2TxdBuffer + Usart2BufferSum;
    Usart2DataQueue.Txd.In = Usart2DataQueue.Txd.Start;
    Usart2DataQueue.Txd.Out = Usart2DataQueue.Txd.Start;
    Usart2DataQueue.Txd.Size = Usart2BufferSum;
    Usart2DataQueue.Txd.Entries = 0;
}
static void InitializeUsart3Queue(void)
{
    Usart3DataQueue.Txd.Start = Usart3TxdBuffer;
    Usart3DataQueue.Txd.End = Usart3TxdBuffer + Usart3BufferSum;
    Usart3DataQueue.Txd.In = Usart3DataQueue.Txd.Start;
    Usart3DataQueue.Txd.Out = Usart3DataQueue.Txd.Start;
    Usart3DataQueue.Txd.Size = Usart3BufferSum;
    Usart3DataQueue.Txd.Entries = 0;
}
        
void InitializeUsart(void) 
{
//GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
//USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
//USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
  
//USART 初始化设置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
   
    USART_Cmd(USART1, ENABLE);                    //使能串口 

    InitializeUsartQueue();

    System.Device.Usart1.Open = OpenUsart1;
    System.Device.Usart1.Close = CloseUsart1;
    System.Device.Usart1.RxdRegister = Usart1RxdRegister;
    System.Device.Usart1.WriteByte = WriteByteToUsart1;
    System.Device.Usart1.Write = WriteToUsart1;
}

//串口2用于RS485
void InitializeUsart2(void) 
{
//GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
//USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
  
//USART 初始化设置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
   
    USART_Cmd(USART2, ENABLE);                    //使能串口 



    //串口2会用于RS485，还有一个接口PA1用于控制,这里在初始话的时候记得控制
    //PA1用与RS485_EN

    
    InitializeUsart2Queue();

    System.Device.Usart2.Open = OpenUsart2;
    System.Device.Usart2.Close = CloseUsart2;
    System.Device.Usart2.RxdRegister = Usart2RxdRegister;
    System.Device.Usart2.WriteByte = WriteByteToUsart2;
    //System.Device.Usart1.Write = WriteToUsart1;
}


void InitializeUsart3(void) 
{
//GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
//USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
//USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
  
//USART 初始化设置
    USART_InitStructure.USART_BaudRate = 38400;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3,USART_IT_RXNE, ENABLE);//开启中断
   
    USART_Cmd(USART3, ENABLE);                    //使能串口 

    InitializeUsart3Queue();

    System.Device.Usart3.Open = OpenUsart3;
    System.Device.Usart3.Close = CloseUsart3;
    System.Device.Usart3.RxdRegister = Usart3RxdRegister;
    System.Device.Usart3.WriteByte = WriteByteToUsart3;
    //System.Device.Usart1.Write = WriteToUsart1;

    
}

