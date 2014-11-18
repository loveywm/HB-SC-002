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

        uint SerialNumber;              //SN
        uint ProductionDate;            //PD
        uint IdentifyNumber0;           //ID0
        uint IdentifyNumber1;           //ID1
        uint IdentifyNumber2;           //ID2
    }Data;

   u16  Input_Data;//16位状态值检测存储值

   
}AppStruct;

extern AppStruct App;

#include "logic.h"
#include "Uart.h"
#include "Proc.h"

//#include "menu.h"

#endif /*__APP_H*/

