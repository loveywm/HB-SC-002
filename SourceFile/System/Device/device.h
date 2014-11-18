/*
********************************************************************************

********************************************************************************
*文件名     : device.h
*作用       : 定义设备层相关宏定义
*原理       : 
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2014/06/21       初始版本
********************************************************************************
*/


#ifndef __DEVICE_H
#define __DEVICE_H



typedef enum 
{
    AddKey                  = 0x00,
    AddLongKey              = 0x30,
    
    SubKey                  = 0x04,
    SubLongKey              = 0x34,

    AssistUpKey             = 0x01,
    AssistUpLongKey         = 0x31,

    AssistDownKey           = 0x05,
    AssistDownLongKey       = 0x35,
    
    SaveKey                 = 0x02,
    SaveLongKey             = 0x32,

    TextBoxFocusKey         = 0x06,
    FormFocusLongKey        = 0x36,
    
    StartKey                = 0x03,
    StartLongKey            = 0x33,
    
    StopKey                 = 0x07,
    StopLongKey             = 0x37
}KeyEnum;

typedef enum 
{
    TimerSystickHandle      = 0,
    TimerMessageHandle      = 1	
}TimerhandleModeEnum;	

typedef struct 
{
    byte Second;
    byte Minute;
    byte Hour;
    uint Day;
} RtcStruct;

typedef enum
{
    AdcChannel0             = 0,
    AdcChannel1             = 1,
    AdcChannel2             = 2,
    AdcChannel3             = 3
}AdcChannelEnum;

typedef struct
{
    ushort Entries;             //存入参数个数
    ushort Size;                //已用参数空间
    ushort Space;               //总参数空间
}ParameterStruct;

typedef struct
{
    char * ReadPointer;         //读取点
    char * WritePointer;        //写入点
    ushort Size;                //已用日志空间
    ushort Space;               //总日志空间
}LogStruct;

typedef enum
{
    Systick10000,
    Systick1000,
    Systick100
}SystickEnum;

#endif

