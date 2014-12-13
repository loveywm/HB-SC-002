/*
********************************************************************************

*文件名     : device_rtc.c
*作用       : 虚拟时钟设备，替代32.768KHz晶体产生的时钟，一般用于精度不高场合
*原理       : 利用系统节拍产生的时钟分频后得到时、分、秒，此精度不高，用于一般场合
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/

#include "system.h"



/*******************************************************************************
* 函数名	: RtcSystickRoutine
* 描述	    : Rtc系统时钟100/S，即10mS一次调用
* 输入参数  : 
* 返回参数  : 
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/10       初始版本
*******************************************************************************/
void RtcSystick100Routine(void) 
{
    static byte Counter = 0;
    
    if(++Counter == 100)
    {
        Counter = 0;

        if (System.Device.Rtc.Second < 59)
            System.Device.Rtc.Second++;
        else
        {
            EnterCritical();
            System.Device.Rtc.Second = 0;
            if(System.Device.Rtc.Minute < 59)
                System.Device.Rtc.Minute++;
            else
            {
                System.Device.Rtc.Minute = 0;
                if(System.Device.Rtc.Hour < 23)
                    System.Device.Rtc.Hour++;
                else
                {
                    System.Device.Rtc.Hour = 0;
                    System.Device.Rtc.Day++;
                } 
            }
            ExitCritical();
        }
    }	
}

void InitializeRtc(void) 
{
        System.Device.Rtc.Second = 0;
        System.Device.Rtc.Minute = 0;
        System.Device.Rtc.Hour = 0;
        System.Device.Rtc.Day = 0;
}

