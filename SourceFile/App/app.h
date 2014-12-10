/*
********************************************************************************
*                   
********************************************************************************
*文件名     : app.h
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/13       初始版本
********************************************************************************
*/


#ifndef __APP_H
#define __APP_H

/*******************************************************************************
* 结构体名	: AppStruct
* 描述	    : msOS中最重要的一个结构体，封装了App层的所有数据及窗口控件
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
typedef unsigned short T_U16;
typedef unsigned short u16;
typedef unsigned char T_U8;
typedef unsigned char u8;
typedef unsigned char BOOL;
typedef unsigned char UINT8,u8;
typedef unsigned short UINT16;
typedef unsigned int UINT32,u32;



#define   MAX_FRAM_LEN    32   /*若帧长度大于此值，则认为无效，重新搜索帧头*/




typedef struct
{
    struct Data
    {
        int Frequency;                 //Frq
        int PowerPercent;
        int Power;
        int Voltage;
        int Current;
        int  Temperature;               
        int State;

        int MaxPower;
        int  MaxTemperature;
        int MaxFrequency;
        int MaxFrequencyOffset;
        int MinStream;

        u32 SerialNumber;              //SN
        u32 ProductionDate;            //PD
        u32 IdentifyNumber0;           //ID0
        u32 IdentifyNumber1;           //ID1
        u32 IdentifyNumber2;           //ID2
    }Data;

   u16  Input_Data;//16位状态值检测存储值
   u32  Weight;//传感器的捕捉的重量值

   u16  Weight_Send;//实际用于发送重量的值
	
}AppStruct;

#include "logic.h"
#include "Uart.h"
#include "proc.h"


extern AppStruct App;
//#include "menu.h"
extern unsigned char fram_len;
extern unsigned char  fram_data_buff[MAX_FRAM_LEN];



#endif /*__APP_H*/

