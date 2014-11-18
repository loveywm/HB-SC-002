
#include "system.h"
#include "app.h"

#define IdentifyNumberAddress  0x1FFFF7E8

#define LogicTaskStackSum   400             //业务逻辑任务栈深度
#define MenuTaskStackSum    100             //菜单界面任务栈深度

uint  LogicTaskStack[LogicTaskStackSum];    //业务逻辑任务栈数组
uint  MenuTaskStack[MenuTaskStackSum];      //菜单界面任务栈数组

void * LogicTaskMessageQueue[40];           //业务逻辑消息队列

AppStruct App;

/*******************************************************************************
* 函数名	: SystemTickRoutine
* 描述	    : 应用层系统节拍，用于执行固定间隔功能，间隔时间取决于注册函数
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/03       初始版本
*******************************************************************************/
static void SystickRoutine(void) 
{
    //回调函数，10ms一次被系统时钟调用

        //10ms检测一下16状态值，跟新保存到对应的全局变量里
        App.Input_Data = Scan_Input_Value();
}


/*******************************************************************************
* 函数名	: Usart1RxdFunction
* 描述	    : Usart1接收处理回调函数，需要通过注册才能使用
* 输入参数  : data Usart接收值
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/13       初始版本
*******************************************************************************/
static void Usart1RxdFunction(byte data)
{
    PostMessageToLogicTask(KeyMessageType, data);  
}


static void Usart3RxdFunction(byte data)
{
        //PostMessageToLogicTask(KeyMessageType, data); 
        //将data中的值传给应用层
        Usart3RxDataTOApp(data);
}

/*******************************************************************************
* 函数名	: InitializeData
* 描述	    : 初始化默认参数
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/08       初始版本
*******************************************************************************/
void InitializeData(void)
{
    /*
    ParameterStruct parameter;
    LogStruct log;

    log = System.Device.Storage.Log.Profile();
    printf("\n日志总存储空间 = %d", log.Space);
    printf("\n日志已存储空间  = %d", log.Size);
    printf("\n日志读取点 = %x", (uint)(log.ReadPointer));
    printf("\n日志写入点 = %x", (uint)(log.WritePointer));
    
    parameter = System.Device.Storage.Parameter.Profile();

    printf("\n\n参数总存储空间 = %d", parameter.Space);
    printf("\n参数已存储空间  = %d", parameter.Size);
    printf("\n参数存储个数 = %d\n", parameter.Entries);
    
    if (parameter.Entries < parameter.Size)
    {
        System.Device.Storage.Parameter.Clean(parameter);
    }
    
    App.Data.Frequency = 900000;
    System.Device.Storage.Parameter.Read(&App.Data.Frequency);
    App.Data.PowerPercent = 20;
    System.Device.Storage.Parameter.Read(&App.Data.PowerPercent);

    App.Data.MaxPower = 6000;
    System.Device.Storage.Parameter.Read(&App.Data.MaxPower);
    App.Data.MaxTemperature = 50;
    System.Device.Storage.Parameter.Read(&App.Data.MaxTemperature);
    App.Data.MaxFrequency = 980000;
    System.Device.Storage.Parameter.Read(&App.Data.MaxFrequency);
    App.Data.MaxFrequencyOffset = 100000;
    System.Device.Storage.Parameter.Read(&App.Data.MaxFrequencyOffset);
    App.Data.MinStream = 50;
    System.Device.Storage.Parameter.Read(&App.Data.MinStream);

    App.Data.SerialNumber = 0;
    System.Device.Storage.Parameter.Read(&App.Data.SerialNumber);
    App.Data.ProductionDate = (13 << 16) + (8 << 8) + 28;
    System.Device.Storage.Parameter.Read(&App.Data.ProductionDate);


    App.Data.Power = 6000;
    App.Data.Voltage = 99;
    App.Data.Current = 101;
    App.Data.Temperature = 25;
    App.Data.State = 1;

    App.Data.IdentifyNumber0 = GetUintFrom(IdentifyNumberAddress);
    App.Data.IdentifyNumber1 = GetUintFrom(IdentifyNumberAddress + 4);
    App.Data.IdentifyNumber2 = GetUintFrom(IdentifyNumberAddress + 8);

    */
}

/*******************************************************************************
* 函数名	: InitializeApp
* 描述	    : 初始化App
*           : InitializeMmi:初始化界面
*           : System.Device.SystemTick.Register:注册应用层节拍
*           : System.Device.Adc.Register:注册ADC参数设备变量
*           : System.Device.Usart1.RxdRegister Usart1接收函数注册
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/03       初始版本
*******************************************************************************/
static void InitializeApp(void)
{
    InitializeData();
    
    //InitializeMenu();

    System.Device.Systick.Register(Systick100, SystickRoutine);

    System.Device.Adc.Register(AdcChannel0, (ushort *)(&App.Data.Voltage));
    System.Device.Adc.Register(AdcChannel1, (ushort *)(&App.Data.Current));

    System.Device.Usart1.RxdRegister(Usart1RxdFunction);

    System.Device.Usart3.RxdRegister(Usart3RxdFunction);
}


/*******************************************************************************
* 函数名	: main
* 描述	    : msOS整个程序入口
*           : InitializeData:初始化存储数据区
*           : System.Initialize:System初始化，实现底层系统初始化 
*           : 任务初始化优先的，优先级也优先，TaskSum要配套设置
*           : LogicTask: 业务逻辑任务，初始化为最高优先级 
*           : MenuTask: 界面任务，初始化为最低优先级,注意，它必须为最低优先级
*           : LogicTaskMessageQueuePoint:初始化一个逻辑任务消息队列
* 输入参数  : 无
* 返回参数  : int
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/03       初始版本
*******************************************************************************/
int main(void) 
{            
        
        System.Initialize();                                                //初始化系统层
        
        InitializeApp();                                                    //初始化应用层



        while(1)
        {

               if(App.Input_Data != Input_invalid)
                {
                        //串口将这个值发送到服务端
                        
               } 

                //将adc值发送到服务端


                //将坐标值发送到服务端


                //接收服务端数据

        }
                                                        //创建业务逻辑任务，最高优先级0 
    //System.OS.CreateTask(LogicTask, &LogicTaskStack[LogicTaskStackSum - 1]);
                                                        //创建菜单界面任务，最低优先级1
    //System.OS.CreateTask(MenuTask, &MenuTaskStack[MenuTaskStackSum - 1] );
                                                        //创建业务逻辑任务的消息队列
    //LogicTaskMessageQueuePointer = System.OS.CreateMessageQueue(LogicTaskMessageQueue, 40);

    //System.OS.Start();                                                  //RTOS启动
}



