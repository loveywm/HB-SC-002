
#include "system.h"
#include "os.h"

//*********************************************************************************************************
//任务状态字 TASK STATUS (state的位定义)       
#define  TaskStatusReady            0x00        					/* 处于就绪状态   */                         
#define  TaskStatusMessageBox       0x02        					/* 处于邮箱状态  */
#define  TaskStatusMessageQueue     0x04        					/* 处于Q状态*/
#define  TaskStatusSuspend			0x08        					/* 处于挂起状态*/


//*********************************************************************************************************
//事件类型(OS_EVENT types)
#define  EventTypeUnused			0        						/* 无事件类型*/
#define  EventTypeMessageBox	   	1							    /* 邮箱为数字序列*/
#define  EventTypeMessageQueue      2							    /* 消息队列为数字序列*/

#if TaskSum == 2        //双任务查找表，适合双任务下的msOS
static const byte UnmapTable[] = 
{
    0, 0, 1, 0
};
#else                   //多任务查找表，兼容uC/OS-II风格
static const byte UnmapTable[] = 
{
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x00 to 0x0F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x10 to 0x1F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x20 to 0x2F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x30 to 0x3F */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x40 to 0x4F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x50 to 0x5F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x60 to 0x6F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x70 to 0x7F */
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x80 to 0x8F */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x90 to 0x9F */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xA0 to 0xAF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xB0 to 0xBF */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xC0 to 0xCF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xD0 to 0xDF */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xE0 to 0xEF */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0        /* 0xF0 to 0xFF */
};
#endif

typedef struct 
{
    uint      * StackTopPointer;    //当前TCB的栈顶指针
    Event     * EventPointer;       //定义指向事件控制块的指针
    void      * MessagePointer;     //消息指针
    uint        Delay;              //任务等待时的最多节拍数
    byte        State;              //任务状态字
    byte        Priority;           //任务优先级(0为最高, 7为最低) 
} Task;

 
static uint Time = 0;                   				/*系统时钟*/
static byte LockNesting = 0;            			    /*锁定嵌套计数器*/
static byte InterruptNesting = 0;				        /*定义中断嵌套层数*/
static byte PriorityReadyTable = 0;		                /*就绪表*/
static byte TaskCounter = 0;					        /*任务计数器*/
static byte EventCounter = 0;				            /*事件计数器*/
static byte QueueCounter = 0;				            /*队列计数器*/
static Task  TaskBlock[TaskSum];				        /*任务控制块*/
static Event EventBlock[EventSum];			            /*事件控制块*/
static Queue QueueBlock[QueueSum];                       /*消息队列控制块*/

bool    OsRunning = false;                       		/*多任务已经开始=1,任务处于不运行状态=0*/
byte    OsCurrentPriority = 0;                		/*定义正在运行的任务的优先级*/
byte    OsHighReadyPriority = 0;			            /*定义具有最高优先级别的就绪任务的优先级*/
Task *	OsCurrentTaskPointer = (Task *)0;		        /*定义指向正在运行任务控制块的指针*/
Task *	OsHighReadyTaskPointer = (Task *)0;	            /*定义指向最高级优先级就绪任务控制块的指针*/


static uint * InitializeTaskStack (void  (*taskPointer)(void), uint  *stackTopPointer)
{
    *(stackTopPointer) = (uint)0x01000000L;           /*  xPSR                        */ 
    *(--stackTopPointer) = (uint)taskPointer;           /*  Entry Point of the task     */
                                                    /*  任务入口地址                */
    *(--stackTopPointer) = (uint)0xFFFFFFFEL;         /*  R14 (LR)  (init value will  */
                                                    /*  cause fault if ever used)   */
                                                                           
    *(--stackTopPointer) = (uint)0x12121212L;         /*  R12                         */
    *(--stackTopPointer) = (uint)0x03030303L;         /*  R3                          */
    *(--stackTopPointer) = (uint)0x02020202L;         /*  R2                          */
    *(--stackTopPointer) = (uint)0x01010101L;         /*  R1                          */
    *(--stackTopPointer) = (uint)0x00000000L;         /*  R0 : argument  输入参数     */
                                                    /*  Remaining registers saved on*/
                                                    /*  process stack               */   
                                                    /*  剩下的寄存器保存到堆栈      */

    *(--stackTopPointer) = (uint)0x11111111L;         /*  R11                         */
    *(--stackTopPointer) = (uint)0x10101010L;         /*  R10                         */
    *(--stackTopPointer) = (uint)0x09090909L;         /*  R9                          */
    *(--stackTopPointer) = (uint)0x08080808L;         /*  R8                          */
    *(--stackTopPointer) = (uint)0x07070707L;         /*  R7                          */
    *(--stackTopPointer) = (uint)0x06060606L;         /*  R6                          */
    *(--stackTopPointer) = (uint)0x05050505L;         /*  R5                          */
    *(--stackTopPointer) = (uint)0x04040404L;         /*  R4                          */

    return(stackTopPointer);
}


extern void     OsSwitchContext(void);

extern void     OsStartHighReadyTask(void);

static void InitializeTask (uint *stackTopPointer)
{
    Task * taskPointer;
		
    taskPointer = &TaskBlock[TaskCounter];
	
    taskPointer->StackTopPointer  = stackTopPointer;                       	/* Load Stack pointer in TCB                */
    taskPointer->EventPointer     = (Event *)0;              			/* Task is not pending on an event          */
    taskPointer->MessagePointer   = null;                			/* No message received                      */
    taskPointer->Delay          = 0;                          			/* Task is not delayed                      */
    taskPointer->State          = TaskStatusReady;                	/* Task is ready to run                     */
    taskPointer->Priority       = TaskCounter;

    PriorityReadyTable  |= (1 << TaskCounter);
    TaskCounter++;
}


  
static void Schedule (void)
{ 
    EnterCritical();
    if ((InterruptNesting == 0) && (LockNesting == 0))
    {
        OsHighReadyPriority = UnmapTable[PriorityReadyTable];//获取最高优先级
        if (OsHighReadyPriority != OsCurrentPriority)//检测当前是否最高运行级
        {                                            //赋值最高优先级任务指针
            OsHighReadyTaskPointer = &TaskBlock[OsHighReadyPriority];
            OsSwitchContext();                                     //任务切换
        }
    }
    ExitCritical();
}

static void EnterInterrupt (void)
{
    Assert(InterruptNesting < 255);
    if (OsRunning == true) 
    {
        InterruptNesting++;
    }
}

static void ExitInterrupt (void)
{
    if (OsRunning == true) 
    {
        EnterCritical();
        if (InterruptNesting > 0) 
        {                       
            InterruptNesting--;
        }
        if ((InterruptNesting == 0) && (LockNesting == 0)) 
        {
            OsHighReadyPriority = UnmapTable[PriorityReadyTable];
					
            if (OsHighReadyPriority != OsCurrentPriority) 
            {              //3
                OsHighReadyTaskPointer = &TaskBlock[OsHighReadyPriority];//4
                OsSwitchContext();
            }
        }
        ExitCritical();	
    }
}

static void SystemTickRoutine(void)
{ 
    byte i = 0;
    Task *taskPointer; 
    
    EnterInterrupt();
    if (OsRunning == true) 
    {                                    
        while (i < TaskSum)
        {          
            taskPointer = &TaskBlock[i];  
            EnterCritical();
            Time++;
            if (taskPointer->Delay != 0) 
            {
                if (--taskPointer->Delay == 0) 
                {
                    if ((taskPointer->State & TaskStatusSuspend) == TaskStatusReady) 
                        PriorityReadyTable |= (1 << i);
                    else 
                        taskPointer->Delay = 1;
                }
            }
            i++;
            ExitCritical();
        }
    }
    ExitInterrupt();
}

static void Start (void)
{
    //断言确认任务数、事件数及队列数，防止错误
    Assert(TaskCounter == TaskSum);
    Assert(EventCounter == EventSum);
    Assert(QueueCounter == QueueSum);
    //注册OS节拍函数
    System.Device.Systick.Register(Systick1000, SystemTickRoutine);
    
    if (OsRunning == false) 
    {
    //找出最高优先级任务
	    OsHighReadyPriority = UnmapTable[PriorityReadyTable];    
        OsCurrentPriority= OsHighReadyPriority;   
    //找出任务块
        OsHighReadyTaskPointer= &TaskBlock[OsCurrentPriority];
        OsCurrentTaskPointer = OsHighReadyTaskPointer;
    //最高优先级任务启动
        OsStartHighReadyTask();                 
    }
}


/*******************************************************************************
* 函数名	: CreateTask
* 描述	    : 创建任务，根据创建的顺序决定优先级，越早创建的，优先级越高，
*           : 第一个为0优先级,注意MenuTask必须为最低优先级，也就是最后一个创建。
*           : 创建了多少个任务，TaskSum要设置为对应的值。
* 输入参数  : taskPointer: 任务函数指针
*           : stackTopPointer: 任务栈顶指针
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void CreateTask (void (*taskPointer)(void), uint *stackTopPointer)
{
    uint * sp;

    EnterCritical();
		
    sp = InitializeTaskStack(taskPointer, stackTopPointer);
    InitializeTask(sp);

    ExitCritical();
}

/*******************************************************************************
* 函数名	: CreateMessageQueue
* 描述	    : 创建一个消息队列，消息队列可以存储多个信息，占用一个事件块及
*             一个队列,EventSum等于MessageBox与MessageQueue两者的总数,QueueSum
*             等于MessageQueue总数。
* 输入参数  : start:队列指针头，size，队列长度
* 返回参数  : 事件块指针
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static Event * CreateMessageQueue (void * * start, ushort size)
{
    Queue * queuePointer;
    Event * eventPointer;
		
    queuePointer = &QueueBlock[QueueCounter];
    queuePointer->Start        = start;
    queuePointer->End          = &start[size];
    queuePointer->In           = start;
    queuePointer->Out          = start;
    queuePointer->Size         = size;
    queuePointer->Entries      = 0;	

    QueueCounter++;
		
    eventPointer = &EventBlock[EventCounter];
    eventPointer->Type = EventTypeMessageQueue;
    eventPointer->Counter = 0;
    eventPointer->MessagePointer = queuePointer;
    eventPointer->PriorityTable = 0;

    EventCounter++;
    			
    return (eventPointer);
}

/*******************************************************************************
* 函数名	: PendMessageQueue
* 描述	    : 等待消息队列，当消息队列为空时，所在任务挂起
* 输入参数  : eventPointer 队列事件块指针，timeout 等待时间，1mS为单位
* 返回参数  : 
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void * PendMessageQueue (Event *eventPointer, uint timeout)
{
    void * messagePointer;
    Queue * queuePointer;

    Assert(InterruptNesting == 0);

    EnterCritical();
		
    queuePointer = (Queue *)eventPointer->MessagePointer;             	/* Point at queue control block                       */
    if (queuePointer->Entries > 0) 							        /* See if any messages in the queue                   */
    {                    
       	messagePointer = *queuePointer->Out++;                     		/* Yes, extract oldest message from the queue         */
       	queuePointer->Entries--;                        				/* Update the number of entries in the queue          */
       	if (queuePointer->Out == queuePointer->End) 
        {          
            queuePointer->Out = queuePointer->Start;			        /* Wrap OUT pointer if we are at the end of the queue */
        }
       	ExitCritical();
       	return (messagePointer);                            			/* Return message received                            */
    }
    OsCurrentTaskPointer->State |= TaskStatusMessageQueue;		        /* Task will have to pend for a message to be posted  */
    OsCurrentTaskPointer->Delay   = timeout;              			    /* Load timeout into TCB                              */
    OsCurrentTaskPointer->EventPointer = eventPointer;
    PriorityReadyTable &= ~(1 <<  OsCurrentPriority);
    eventPointer->PriorityTable |= 1 <<  OsCurrentPriority;
		
    ExitCritical();
    Schedule();                                  					/* Find next highest priority task ready to run       */
    EnterCritical();
    messagePointer = OsCurrentTaskPointer->MessagePointer;
    if ((messagePointer != null) || (timeout == 0))                   /*Wangsw add || (timeout == 0) for direct DataQueue */
    {                      										    /* Did we get a message?                              */
        OsCurrentTaskPointer->MessagePointer      = null;     		    /* Extract message from TCB (Put there by QPost)      */
        OsCurrentTaskPointer->State     = TaskStatusReady;
        OsCurrentTaskPointer->EventPointer = (Event *)0; 			        /* No longer waiting for event                        */
        ExitCritical();
        return (messagePointer);                            		    /* Return message received                            */
    }
    	
    eventPointer->PriorityTable &= ~(1 << OsCurrentPriority);
    OsCurrentTaskPointer->State = TaskStatusReady;
    OsCurrentTaskPointer->EventPointer = (Event *)0;
    ExitCritical();
    return (null);                          					    /* No message received                                */
}



/*******************************************************************************
* 函数名	: PostMessageQueue
* 描述	    : 发送一个消息到消息队列中，处于等待的任务会自动运行
* 输入参数  : eventPointer 队列事件块指针，messagePointer发送消息指针
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static byte PostMessageQueue (Event *eventPointer, void *messagePointer)
{
    byte eventHighPriority;
    Queue * queuePointer;
    Task * taskPointer;
	
    EnterCritical();
    if (eventPointer->PriorityTable != 0x00) 				            /* See if any task pending on queue              */
    {                 
        eventHighPriority = UnmapTable[eventPointer->PriorityTable];
        eventPointer->PriorityTable &= ~(1 << eventHighPriority);

        taskPointer = &TaskBlock[eventHighPriority];
        taskPointer->Delay = 0;
        taskPointer->EventPointer = (Event *)0;
        taskPointer->MessagePointer = messagePointer;
        taskPointer->State &= ~TaskStatusMessageQueue;
        if (taskPointer->State == TaskStatusReady)
        {
            PriorityReadyTable |=  1 << eventHighPriority;
        }
		
        ExitCritical();
        Schedule();                                                 /* Find highest priority task ready to run       */
        return (true);
    }
    queuePointer = (Queue *)eventPointer->MessagePointer;                 /* Point to queue control block                  */
    if (queuePointer->Entries >= queuePointer->Size) 
    {                                                               /* Make sure queue is not full                   */
        ExitCritical();
        return (false);
    }
    *queuePointer->In++ = messagePointer;                               /* Insert message into queue                     */
    queuePointer->Entries++;                                          /* Update the nbr of entries in the queue        */
    if (queuePointer->In == queuePointer->End) 
    {                                                               /* Wrap IN ptr if we are at end of queue         */
       	queuePointer->In = queuePointer->Start;
    }
    ExitCritical();
    return (true);
}



/*******************************************************************************
* 函数名	: DelayTimeTick
* 描述	    : 任务挂起等待时间，单位为1mS
* 输入参数  : timeTick，挂起时间，1mS为单位，比如1000,就是1秒钟
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
static void DelayTimeTick (uint timeTick)
{
    if (timeTick > 0) 
    {                                                   
       	EnterCritical();
        PriorityReadyTable &= ~(1 << OsCurrentPriority);
       	OsCurrentTaskPointer->Delay = timeTick;
       	ExitCritical();
       	Schedule();
    }
}

void InitializeOs(void)
{
    System.OS.Start = Start;
    System.OS.CreateTask = CreateTask;
    System.OS.CreateMessageQueue= CreateMessageQueue;
    System.OS.PendMessageQueue= PendMessageQueue;
    System.OS.PostMessageQueue= PostMessageQueue;
    System.OS.DelayTimeTick= DelayTimeTick;
}














