/*
********************************************************************************

*文件名     : device_timer.c
*作用       : 软件定时器设备
*原理       : 基于系统节拍1000/S，即1mS一次虚拟出软件定时器
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/



#include "system.h"
#include "drive.h"




typedef struct 
{
    uint  Delay;
    function RegisterFunction;
}Timer;

#define TimerSum		0x8

static uint  State = 0;
static TimerhandleModeEnum Mode;
static Timer Block[TimerSum];


/*******************************************************************************
* 函数名	: TimerSystick1000Routine
* 描述	    : 系统节拍1000/S,即1mS一次调用，实现虚拟定时器。
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/10       初始版本
*******************************************************************************/
void TimerSystick1000Routine(void)
{
    byte i = 0;
    uint stateCopy;
    
    if(State == 0x00) 
    {
        return;
    }
	
    EnterCritical();
	
    stateCopy = State;
    while(stateCopy)	
    {
        if((stateCopy & 0x01) == 1) 
        {
            if((--Block[i].Delay) == 0) 
            {
                if(GetBit(Mode, i)) 
                {
                    PostMessageToLogicTask(TimerMessageType, (uint)(Block[i].RegisterFunction));
                } 
                else 
                {
                    (*(Block[i].RegisterFunction))(); 
                }
                ResetBit(State, i);
            }
        }
		
        stateCopy = stateCopy >> 1;
        i++;
    }
    ExitCritical();
}

/*******************************************************************************
* 函数名	: Start
* 描述	    : 软件定时器
* 输入参数  : handleMode: 两种处理方式，一种直接在节拍中断中处理，适合费用低的,
*                         另一种在消息中处理，适合处理费用高的。
*           : delay:延时节拍数，以系统节拍为单位
*           : registerFunction: 注册回调执行函数，延时超时后，执行此函数。
*             data:消息值，MessageTimer类型为32bit地址,其他类型下都是24bit数据
* 返回参数  : byte类型，返回ID号，从0开始，若失败则返回invalid(-1)
*******************************************************************************/
static byte Start(TimerhandleModeEnum handleMode, uint delay, function registerFunction)
{
    byte i;

    EnterCritical();
	
    for(i = 0; i < TimerSum; i++) 
    {
        if(!GetBit(State, i)) 
        {			
            Block[i].Delay = delay;/*延时时间*/
            Block[i].RegisterFunction = registerFunction;/*回调函数*/
            if(handleMode) 
            {
                SetBit(Mode, i);
            } 
            else 
            {
                ResetBit(Mode, i);
            }
			
            SetBit(State, i);
            ExitCritical();
            return(i);
        }
    }
    ExitCritical();
    return(invalid);
}


/*******************************************************************************
* 函数名	: Stop
* 描述	    : 停止某一路的软件定时器
* 输入参数  : id为0、1、2...
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/10       初始版本
*******************************************************************************/
static void Stop(byte id) 
{
    Assert(id < TimerSum);
    
    EnterCritical();
	
    ResetBit(State, id);
	
    ExitCritical();
}


void InitializeTimer(void)
{
        System.Device.Timer.Start = Start;
        System.Device.Timer.Stop = Stop; 
}
