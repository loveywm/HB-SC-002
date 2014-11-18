/*
********************************************************************************
*                   
********************************************************************************
*�ļ���     : app.h
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/13       ��ʼ�汾
********************************************************************************
*/


#ifndef __APP_H
#define __APP_H

/*******************************************************************************
* �ṹ����	: AppStruct
* ����	    : msOS������Ҫ��һ���ṹ�壬��װ��App����������ݼ����ڿؼ�
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
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

   u16  Input_Data;//16λ״ֵ̬���洢ֵ

   
}AppStruct;

extern AppStruct App;

#include "logic.h"
#include "Uart.h"
#include "Proc.h"

//#include "menu.h"

#endif /*__APP_H*/

