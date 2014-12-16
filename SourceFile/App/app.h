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
typedef unsigned short T_U16;
typedef unsigned short u16;
typedef unsigned char T_U8;
typedef unsigned char u8;
typedef unsigned char BOOL;
typedef unsigned char UINT8,u8;
typedef unsigned short UINT16;
typedef unsigned int UINT32,u32;



#define   MAX_FRAM_LEN    32   /*��֡���ȴ��ڴ�ֵ������Ϊ��Ч����������֡ͷ*/

#define     MAX_FLOOR_NUM   50//��ʱ����¥�����߶�

typedef struct{
        u8  floor_flag;//Ϊ1ʱ��ʾ��ʹ�ã�Ϊ0ʱ��ʾ��ֵ������
        u8  floor_num;//��ֵ¥��
        u32     floor_count;//¥���Ӧ�ı���������ֵ
}Floor_Data;


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

   u16  Input_Data;//16λ״ֵ̬���洢ֵ
   u32  Weight;//�������Ĳ�׽������ֵ

   u16  Weight_Send;//ʵ�����ڷ���������ֵ
	
}AppStruct;

#include "logic.h"
#include "Uart.h"
#include "proc.h"


extern AppStruct App;
//#include "menu.h"
extern unsigned char fram_len;
extern unsigned char  fram_data_buff[MAX_FRAM_LEN];



#endif /*__APP_H*/

