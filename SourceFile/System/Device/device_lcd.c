/*
********************************************************************************

********************************************************************************
*�ļ���     : device_lcd.c
*����       : 128*64���ֿ�ڰ����豸
*ԭ��       : 128*64�ֿ�ڰ�������Ϊ���У��ֿ⺺�ֲ���֧��GB2312���룬
*           : Keil������ֱ��֧��GB2312������ÿ��16*16����һ�п�����ʾ8������
*           : ��ĸ֧��ASCII��׼��ÿ��8*16��һ�п�����ʾ16����ĸ
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
********************************************************************************
*/



#include "drive.h"
#include "system.h"


#ifdef DEMO_NEW
#define ClkPin	    PcOut(9)
#define CsPin	    PcOut(8)
#define DataPin	    PcOut(7)
#define ResetPin	PcOut(6)
#endif

#ifdef DEMO_OLD
#define ClkPin	    PaOut(8)
#define CsPin	    PcOut(9)
#define DataPin	    PcOut(8)
#define ResetPin    PcOut(7)
#endif


const char Array[4] = 
{
    0x80, 0x90, 0x88, 0x98
};
static void LcdSendInstruct(byte instruct)
{
    short i;
    uint variable;

    variable = 0x00F80000 | ((instruct & 0xF0) << 8) |((instruct & 0x0F) << 4); 

    ClkPin = 0;	
    CsPin = 1;
    
    for (i = 23; i > -1; i--) 
    {
        if(GetBit(variable, i)) 
        {
            DataPin = 1;
        } 
        else 
        {
            DataPin = 0;
        }
        DelayUs(20);
        ClkPin = 1;
        DelayUs(20);
        ClkPin = 0;
        DelayUs(20);
    }
    CsPin = 0;
}

static void LcdSendData(byte data)
{
    short i;
    uint variable;
    variable = 0x00FA0000 | ((data & 0xF0) << 8) |((data & 0x0F) << 4); 

    ClkPin = 0;
    CsPin = 1;

    for (i = 23; i > -1; i--) 
    {
        if (GetBit(variable, i)) 
        {
            DataPin = 1;
        } 
        else 
        {
            DataPin = 0;
        }
        DelayUs(20);
        ClkPin = 1;
        DelayUs(20);
        ClkPin = 0;
        DelayUs(20);
    }
    CsPin = 0;
}


/*******************************************************************************
* ������	: LcdDisplayString
* ����	    : Lcd����ʾ��������ʾһ����16����ĸ����8�����֣����ֱ���Ҫ��ż��λ�ÿ�ʼ
* �������  : y: �к�0��1��2��3
*           : string �ַ���������Ҫ16��char��һ�����ֵ�������char
* ���ز���  : 
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/09       ��ʼ�汾
*******************************************************************************/
static void LcdDisplayString(byte y, string string)
{ 
    byte i;
    
    Assert(y < 4);
	
    LcdSendInstruct(Array[y]);

    for (i = 0; i < 16; i++)
    {
        Assert(*string >= ' ');
        LcdSendData(*string++);
    }
}

void InitializeLcd(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
#ifdef DEMO_NEW
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    ClkPin = 0;		


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    CsPin = 0;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    DataPin = 0;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    ResetPin = 0;	
#endif
#ifdef DEMO_OLD
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    ClkPin = 0;		


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    CsPin = 0;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    DataPin = 0;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    ResetPin = 0;	
#endif
    DelayMs(100);

    ResetPin = 1;
    LcdSendInstruct(0x30);
    LcdSendInstruct(0x0C);
    LcdSendInstruct(0x01);
    LcdSendInstruct(0x02);
    LcdSendInstruct(0x80);
	
    LcdSendInstruct(0x01);
	
    System.Device.Lcd.DisplayString = LcdDisplayString;
}



