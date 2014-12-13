/*
********************************************************************************

*�ļ���     : device_timer.c
*����       : �����ʱ���豸
*ԭ��       : ����ϵͳ����1000/S����1mSһ������������ʱ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
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
* ������	: TimerSystick1000Routine
* ����	    : ϵͳ����1000/S,��1mSһ�ε��ã�ʵ�����ⶨʱ����
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/10       ��ʼ�汾
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
* ������	: Start
* ����	    : �����ʱ��
* �������  : handleMode: ���ִ���ʽ��һ��ֱ���ڽ����ж��д����ʺϷ��õ͵�,
*                         ��һ������Ϣ�д����ʺϴ�����øߵġ�
*           : delay:��ʱ����������ϵͳ����Ϊ��λ
*           : registerFunction: ע��ص�ִ�к�������ʱ��ʱ��ִ�д˺�����
*             data:��Ϣֵ��MessageTimer����Ϊ32bit��ַ,���������¶���24bit����
* ���ز���  : byte���ͣ�����ID�ţ���0��ʼ����ʧ���򷵻�invalid(-1)
*******************************************************************************/
static byte Start(TimerhandleModeEnum handleMode, uint delay, function registerFunction)
{
    byte i;

    EnterCritical();
	
    for(i = 0; i < TimerSum; i++) 
    {
        if(!GetBit(State, i)) 
        {			
            Block[i].Delay = delay;/*��ʱʱ��*/
            Block[i].RegisterFunction = registerFunction;/*�ص�����*/
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
* ������	: Stop
* ����	    : ֹͣĳһ·�������ʱ��
* �������  : idΪ0��1��2...
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/10       ��ʼ�汾
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
