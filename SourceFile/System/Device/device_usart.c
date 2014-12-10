/*
********************************************************************************
*                                Ƕ��ʽ΢ϵͳ
*                                   msOS
*
********************************************************************************
*�ļ���     : device_usart.c
*����       : �����豸
*ԭ��       : ���Ͳ���Buffer���ͣ����ղ���Appע�ắ����ʽ����
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
*V0.2    Wangsw        2013/09/13       ��һ�θ���
********************************************************************************
*/

#include "drive.h"
#include "system.h"

typedef void (*RxdRegisterFunction)(byte data);

typedef struct 
{
    byte    * Start;    //ָ����п�ʼ 
    byte    * End;      //ָ����н��� 
    byte    * In;       //����һ����Ϣ  
    byte    * Out;      //ȡ��һ����Ϣ  
    ushort    Size;     // ���г��� 
    ushort    Entries;  //��Ϣ����      
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
* ������	: fputc
* ����	    : Keil������֧��printf��������Ҫ���ô˺�����ע�������ҪMicorLib֧��
* �������  : ch ʵΪbyte���ͣ�ʵ�ʷ��͵�����
*           : f �豸���������Ҫ����
* ���ز���  : 
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/10       ��ʼ�汾
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
* ������	: USART1_IRQHandler
* ����	    : Ĭ�ϴ��ڽ����ж���ڣ����ղ���App�ϲ�ע�ắ�����ƣ���App�����д���
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/10       ��ʼ�汾
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
* ������	: UsartSystick1000Routine
* ����	    : ϵͳ����1000/S,��1mSһ�ε��ô˷�������ͻ���������
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/10       ��ʼ�汾
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
* ������	: Usart1RxdRegister
* ����	    : Usart1����ע�ắ������App�����ע����Ҫ�ص�����Usart1���յĺ���
* �������  : registerFunction App����մ�����
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/14       ��ʼ�汾
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
* ������	: WriteByteToUsart1
* ����	    : ����1����һ���ֽ�
* �������  : data �ֽ����ͣ����Ͳ���
* ���ز���  : bool �������ͣ������Ƿ�ɹ�
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/13       ��ʼ�汾
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
* ������	: WriteToUsart1
* ����	    : ����1����һ������
* �������  : dataPointer ����ָ�룬sum ��������
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/13       ��ʼ�汾
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
//GPIO�˿�����
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

//Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
  
//USART ��ʼ������
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
   
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

    InitializeUsartQueue();

    System.Device.Usart1.Open = OpenUsart1;
    System.Device.Usart1.Close = CloseUsart1;
    System.Device.Usart1.RxdRegister = Usart1RxdRegister;
    System.Device.Usart1.WriteByte = WriteByteToUsart1;
    System.Device.Usart1.Write = WriteToUsart1;
}

//����2����RS485
void InitializeUsart2(void) 
{
//GPIO�˿�����
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

//Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
  
//USART ��ʼ������
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
   
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 



    //����2������RS485������һ���ӿ�PA1���ڿ���,�����ڳ�ʼ����ʱ��ǵÿ���
    //PA1����RS485_EN

    
    InitializeUsart2Queue();

    System.Device.Usart2.Open = OpenUsart2;
    System.Device.Usart2.Close = CloseUsart2;
    System.Device.Usart2.RxdRegister = Usart2RxdRegister;
    System.Device.Usart2.WriteByte = WriteByteToUsart2;
    //System.Device.Usart1.Write = WriteToUsart1;
}


void InitializeUsart3(void) 
{
//GPIO�˿�����
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

//Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
  
//USART ��ʼ������
    USART_InitStructure.USART_BaudRate = 38400;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3,USART_IT_RXNE, ENABLE);//�����ж�
   
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 

    InitializeUsart3Queue();

    System.Device.Usart3.Open = OpenUsart3;
    System.Device.Usart3.Close = CloseUsart3;
    System.Device.Usart3.RxdRegister = Usart3RxdRegister;
    System.Device.Usart3.WriteByte = WriteByteToUsart3;
    //System.Device.Usart1.Write = WriteToUsart1;

    
}

