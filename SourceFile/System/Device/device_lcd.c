/*
********************************************************************************

********************************************************************************
*文件名     : device_lcd.c
*作用       : 128*64带字库黑白屏设备
*原理       : 128*64字库黑白屏，分为四行，字库汉字部分支持GB2312编码，
*           : Keil编译器直接支持GB2312，汉字每个16*16点阵，一行可以显示8个汉字
*           : 字母支持ASCII标准，每个8*16，一行可以显示16个字母
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
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
* 函数名	: LcdDisplayString
* 描述	    : Lcd屏显示函数，显示一整行16个字母或者8个汉字，汉字必须要在偶数位置开始
* 输入参数  : y: 行号0、1、2、3
*           : string 字符串，必须要16个char，一个汉字等于两个char
* 返回参数  : 
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/09       初始版本
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



