

#include "system.h"


ushort CriticalNesting = 0;
Event * LogicTaskMessageQueuePointer = (Event *)0;

static const byte HextoAscii[16] = 
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

#define DelayUsTick MainClock / 9000000

#define DelayMsTick MainClock / 9000

void Delay(uint delayClock)
{
	while(delayClock--);
}

void DelayUs(uint delayTime)
{
	while(delayTime--)
	{
		Delay(DelayUsTick);
	}
}

void DelayMs(uint delayTime)
{
	while(delayTime--)
	{
		Delay(DelayMsTick);
	}
}


byte HexToAscii(byte hex)
{
	Assert(hex < 16);

    if (hex < 16) 
        return HextoAscii[hex];
    else 
         return 0x30;
}



/*******************************************************************************
* 函数名	: PostMessageToLogicTask
* 描述	    : 向LogicTask发送消息
* 输入参数  : messageType: 消息类型
*             data:消息值，MessageTimer类型为32bit地址,其他类型下都是24bit数据
* 返回参数  : 无
*******************************************************************************/
void PostMessageToLogicTask(MessageType messageType, uint data)
{
    bool postMessage;

    if (LogicTaskMessageQueuePointer == (Event *)0) return;

    if (messageType != TimerMessageType)
    {
        UintToByte3(data) = messageType;
    }
    
   // postMessage = System.OS.PostMessageQueue(LogicTaskMessageQueuePointer, (void *)data);	
    Assert(postMessage);
}




extern void InitializeDevice(void);

static void Initialize(void)
{
	//InitializeOs();
    //InitializeGui();
	InitializeDevice();
}

SystemStruct System = 
{
	Initialize,
};


