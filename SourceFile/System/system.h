/*
********************************************************************************

********************************************************************************
*文件名   : system.h
*作用     : 系统库全局头文件
*创建时间 : 2013.7.21
********************************************************************************
*/


#ifndef __SYSTEM_H
#define __SYSTEM_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>



#define DEMO_NEW

//#define DEMO_OLD





/*
1、类型定义兼容C#，尽可能用C#标准，常用的为int
2、char在C#中为16bit的unicode，keil C下为8bit
*/
//8bit
typedef char                sbyte;	    /*char 不能用于负数*/
typedef unsigned char       byte;       /*实际测试byte = char*/
//16bit
//typedef short             short
typedef unsigned short      ushort;
//32bit
//typedef int               int;
typedef unsigned int        uint;

//字符串
typedef char *              string;

typedef unsigned char       bool;



typedef unsigned short T_U16;
typedef unsigned short u16;
typedef unsigned char T_U8;
typedef unsigned char u8;
typedef unsigned char BOOL;
typedef unsigned char UINT8,u8;
typedef unsigned short UINT16;
typedef unsigned int UINT32,u32;

typedef void (*function)(void);
#define Function(address)   (*(function)address)()








#define true                1
#define false               0
#define null                ((void *) 0)
#define invalid             0xFF


#define Input_invalid                 0x0

#define Input_invalid_1             0x0001
#define Input_invalid_2             0x0002
#define Input_invalid_3             0x0004
#define Input_invalid_4             0x0008

#define Input_invalid_5             0x0010
#define Input_invalid_6             0x0020
#define Input_invalid_7             0x0040
#define Input_invalid_8             0x0080

#define Input_invalid_9             0x0100
#define Input_invalid_10             0x0200
#define Input_invalid_11             0x0400
#define Input_invalid_12             0x0800

#define Input_invalid_13             0x1000
#define Input_invalid_14             0x2000
#define Input_invalid_15             0x4000
#define Input_invalid_16             0x8000



#define MainClock           72000000

#define SetBit(data, offset)         (data |= 1U << offset)
#define ResetBit(data, offset)       (data &= ~(1U << offset))
#define GetBit(data, offset)         ((data >> offset) &0x01)

#define UintToByte3(data)               ((byte *)(&(data)))[3]
#define UintToByte2(data)               ((byte *)(&(data)))[2]
#define UintToByte1(data)               ((byte *)(&(data)))[1]
#define UintToByte0(data)               ((byte *)(&(data)))[0]

#define UintToUshort1(data)             ((ushort *)(&(data)))[1]
#define UintToUshort0(data)             ((ushort *)(&(data)))[0]

#define GetMessageType(data)            UintToByte3(data)
#define GetMessageData(data)            (data & 0x00FFFFFF)

#define GetByteFrom(address)                *((byte *)(address)) 
#define GetUshortFrom(address)              *((ushort *)(address)) 
#define GetUintFrom(address)                *((uint *)(address)) 

#define Assert(express)  if (!(express)){printf("\nASSERT: " #express "\n");}   //(void)0

enum
{
        Gpio_A,
        Gpio_B,
        Gpio_C,
};
//输出IO口的对外定义
enum{

        LED15,
        LED16,
        LED17,
        LED18,

        OUT501,
        OUT502,
        OUT503,
        OUT504,
        OUT505,
        OUT506,
        
        IN_301,
        IN_302,
        IN_303,
        IN_304,
        IN_305,
        IN_306,


        IN_201,
        IN_202,
        IN_203,
        IN_204,
        IN_205,
        IN_206,
        IN_207,
        IN_208,
        IN_209,
        IN_210,
        
        
        IN301= IN_301,
        IN302= IN_302,
        IN303= IN_303,
        IN304= IN_304,
        IN305= IN_305,
        IN306= IN_306,


        IN1= IN_201,
        IN2= IN_202,
        IN3= IN_203,
        IN4= IN_204,
        IN5= IN_205,
        IN6= IN_206,
        IN7= IN_207,
        IN8= IN_208,
        IN9= IN_209,
        IN10= IN_210,


     
        


};

typedef struct GPIO_Set
{
        u8      Gpio_IO;
        u8      Gpio_Value;

}GPIO_Set;




typedef enum
{
    KeyMessageType      = 0xFF,
    /*请填充类型*/
    TimerMessageType    = 0xF0  /*最小值,不允许有值小于它*/
}MessageType;
 				   
#include "device.h"  
#include "os.h"
#include "gui.h"


extern Event * LogicTaskMessageQueuePointer;

extern ushort CriticalNesting;
#define	EnterCritical()		__disable_irq(); CriticalNesting++;
#define ExitCritical()		if(--CriticalNesting == 0){__enable_irq();}

extern void Delay(uint delayClock);

extern void DelayUs(uint delayTime);

extern void DelayMs(uint delayTime);

extern byte HexToAscii(byte hex);

extern void PostMessageToLogicTask(MessageType messageType, uint data);

/*******************************************************************************
* 结构体名	: SystemStruct
* 描述	    : msOS中最重要的一个结构体，封装了System层的所有接口
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
typedef struct
{
        void (*Initialize)(void);
    
        struct Device
        {      
                RtcStruct Rtc;

                struct IO
                {
                        //void (*SetBeep)(bool status);
                        void (*Set_Led_Value)(u8 lednum,u8 value);
                        void (*Set_Relay_Value)(u8 relay_num,u8 value);
                        void (*HB_Gpio_Set_Value)(u8 HB_Gpio_type,u8 HB_Gpio_Value);
                        u8    (*HB_Gpio_Get_Value)(u8 HB_Gpio_type);
                        u16  (*Scan_External_Input_Value)(void);
                }IO;

                struct Adc
                {
                        void (*Register)(AdcChannelEnum adcChannel, ushort * dataPointer);
                }Adc;
          
        struct Usart1
        {
            void (*Open)(void);
            void (*Close)(void);
            void (*RxdRegister)(void * registerFunction);
            bool (*WriteByte)(byte data);
            void (*Write)(byte * dataPointer, int sum);
        }Usart1;
        struct Usart2
        {
            void (*Open)(void);
            void (*Close)(void);
            void (*RxdRegister)(void * registerFunction);
            bool (*WriteByte)(byte data);
            //void (*Write)(byte * dataPointer, int sum);
        }Usart2;
          struct Usart3
        {
            void (*Open)(void);
            void (*Close)(void);
            void (*RxdRegister)(void * registerFunction);
            bool (*WriteByte)(byte data);
            //void (*Write)(byte * dataPointer, int sum);
        }Usart3;
        
        struct Timer
        {
            byte (*Start)(TimerhandleModeEnum mode, uint delay, function registerFunction);
            void (*Stop)(byte id); 
        }Timer;

        struct Systick
        {
            bool (*Register)(SystickEnum systemTick, function registerFunction);
        }Systick;
        
    }Device;


}SystemStruct;

extern SystemStruct System;

#endif 
