
#include "system.h"
#include "os.h"

//*********************************************************************************************************
//����״̬�� TASK STATUS (state��λ����)       
#define  TaskStatusReady            0x00        					/* ���ھ���״̬   */                         
#define  TaskStatusMessageBox       0x02        					/* ��������״̬  */
#define  TaskStatusMessageQueue     0x04        					/* ����Q״̬*/
#define  TaskStatusSuspend			0x08        					/* ���ڹ���״̬*/


//*********************************************************************************************************
//�¼�����(OS_EVENT types)
#define  EventTypeUnused			0        						/* ���¼�����*/
#define  EventTypeMessageBox	   	1							    /* ����Ϊ��������*/
#define  EventTypeMessageQueue      2							    /* ��Ϣ����Ϊ��������*/

#if TaskSum == 2        //˫������ұ��ʺ�˫�����µ�msOS
static const byte UnmapTable[] = 
{
    0, 0, 1, 0
};
#else                   //��������ұ�����uC/OS-II���
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
    uint      * StackTopPointer;    //��ǰTCB��ջ��ָ��
    Event     * EventPointer;       //����ָ���¼����ƿ��ָ��
    void      * MessagePointer;     //��Ϣָ��
    uint        Delay;              //����ȴ�ʱ����������
    byte        State;              //����״̬��
    byte        Priority;           //�������ȼ�(0Ϊ���, 7Ϊ���) 
} Task;

 
static uint Time = 0;                   				/*ϵͳʱ��*/
static byte LockNesting = 0;            			    /*����Ƕ�׼�����*/
static byte InterruptNesting = 0;				        /*�����ж�Ƕ�ײ���*/
static byte PriorityReadyTable = 0;		                /*������*/
static byte TaskCounter = 0;					        /*���������*/
static byte EventCounter = 0;				            /*�¼�������*/
static byte QueueCounter = 0;				            /*���м�����*/
static Task  TaskBlock[TaskSum];				        /*������ƿ�*/
static Event EventBlock[EventSum];			            /*�¼����ƿ�*/
static Queue QueueBlock[QueueSum];                       /*��Ϣ���п��ƿ�*/

bool    OsRunning = false;                       		/*�������Ѿ���ʼ=1,�����ڲ�����״̬=0*/
byte    OsCurrentPriority = 0;                		/*�����������е���������ȼ�*/
byte    OsHighReadyPriority = 0;			            /*�������������ȼ���ľ�����������ȼ�*/
Task *	OsCurrentTaskPointer = (Task *)0;		        /*����ָ����������������ƿ��ָ��*/
Task *	OsHighReadyTaskPointer = (Task *)0;	            /*����ָ����߼����ȼ�����������ƿ��ָ��*/


static uint * InitializeTaskStack (void  (*taskPointer)(void), uint  *stackTopPointer)
{
    *(stackTopPointer) = (uint)0x01000000L;           /*  xPSR                        */ 
    *(--stackTopPointer) = (uint)taskPointer;           /*  Entry Point of the task     */
                                                    /*  ������ڵ�ַ                */
    *(--stackTopPointer) = (uint)0xFFFFFFFEL;         /*  R14 (LR)  (init value will  */
                                                    /*  cause fault if ever used)   */
                                                                           
    *(--stackTopPointer) = (uint)0x12121212L;         /*  R12                         */
    *(--stackTopPointer) = (uint)0x03030303L;         /*  R3                          */
    *(--stackTopPointer) = (uint)0x02020202L;         /*  R2                          */
    *(--stackTopPointer) = (uint)0x01010101L;         /*  R1                          */
    *(--stackTopPointer) = (uint)0x00000000L;         /*  R0 : argument  �������     */
                                                    /*  Remaining registers saved on*/
                                                    /*  process stack               */   
                                                    /*  ʣ�µļĴ������浽��ջ      */

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
        OsHighReadyPriority = UnmapTable[PriorityReadyTable];//��ȡ������ȼ�
        if (OsHighReadyPriority != OsCurrentPriority)//��⵱ǰ�Ƿ�������м�
        {                                            //��ֵ������ȼ�����ָ��
            OsHighReadyTaskPointer = &TaskBlock[OsHighReadyPriority];
            OsSwitchContext();                                     //�����л�
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
    //����ȷ�����������¼���������������ֹ����
    Assert(TaskCounter == TaskSum);
    Assert(EventCounter == EventSum);
    Assert(QueueCounter == QueueSum);
    //ע��OS���ĺ���
    System.Device.Systick.Register(Systick1000, SystemTickRoutine);
    
    if (OsRunning == false) 
    {
    //�ҳ�������ȼ�����
	    OsHighReadyPriority = UnmapTable[PriorityReadyTable];    
        OsCurrentPriority= OsHighReadyPriority;   
    //�ҳ������
        OsHighReadyTaskPointer= &TaskBlock[OsCurrentPriority];
        OsCurrentTaskPointer = OsHighReadyTaskPointer;
    //������ȼ���������
        OsStartHighReadyTask();                 
    }
}


/*******************************************************************************
* ������	: CreateTask
* ����	    : �������񣬸��ݴ�����˳��������ȼ���Խ�紴���ģ����ȼ�Խ�ߣ�
*           : ��һ��Ϊ0���ȼ�,ע��MenuTask����Ϊ������ȼ���Ҳ�������һ��������
*           : �����˶��ٸ�����TaskSumҪ����Ϊ��Ӧ��ֵ��
* �������  : taskPointer: ������ָ��
*           : stackTopPointer: ����ջ��ָ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
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
* ������	: CreateMessageQueue
* ����	    : ����һ����Ϣ���У���Ϣ���п��Դ洢�����Ϣ��ռ��һ���¼��鼰
*             һ������,EventSum����MessageBox��MessageQueue���ߵ�����,QueueSum
*             ����MessageQueue������
* �������  : start:����ָ��ͷ��size�����г���
* ���ز���  : �¼���ָ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
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
* ������	: PendMessageQueue
* ����	    : �ȴ���Ϣ���У�����Ϣ����Ϊ��ʱ�������������
* �������  : eventPointer �����¼���ָ�룬timeout �ȴ�ʱ�䣬1mSΪ��λ
* ���ز���  : 
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
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
* ������	: PostMessageQueue
* ����	    : ����һ����Ϣ����Ϣ�����У����ڵȴ���������Զ�����
* �������  : eventPointer �����¼���ָ�룬messagePointer������Ϣָ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
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
* ������	: DelayTimeTick
* ����	    : �������ȴ�ʱ�䣬��λΪ1mS
* �������  : timeTick������ʱ�䣬1mSΪ��λ������1000,����1����
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
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














