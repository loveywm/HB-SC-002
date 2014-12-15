
#include "system.h"
#include "app.h"

#include "stm32f10x_gpio.h"


#define N_ADC   32


#define IdentifyNumberAddress  0x1FFFF7E8

#define LogicTaskStackSum   400             //业务逻辑任务栈深度
#define MenuTaskStackSum    100             //菜单界面任务栈深度

uint  LogicTaskStack[LogicTaskStackSum];    //业务逻辑任务栈数组
uint  MenuTaskStack[MenuTaskStackSum];      //菜单界面任务栈数组

void * LogicTaskMessageQueue[40];           //业务逻辑消息队列

AppStruct App;

unsigned char  fram_data_buff[MAX_FRAM_LEN];////这个数组保存得到的一个掐头去尾的报文帧
unsigned char  fram_len = 0;
//u8 weight_tmp_flag = 0;//临时计算重量是否相等的计数标志
u8  RT_Send_Flag = 0;//实时数据是否发送的标志


signed int Floor_CurrentCount = 0;//用于计数编码器的的值 


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
    //回调函数，1ms一次被系统时钟调用

        //10ms检测一下16状态值，跟新保存到对应的全局变量里
        //App.Input_Data = Scan_Input_Value();

        //HB_Send_ErrorAndWeight(App.Input_Data,App.Weight);
                if(Rcv_Cmd() == 1)
                {
			if(fram_data_buff[0] == CMD_RELAY_UP)
			{
				HB_Relay_Cmd(RELAY_1,RELAY_ON);
                                    HB_Relay_Cmd(RELAY_2,RELAY_OFF);
                                    HB_Relay_Cmd(RELAY_4,RELAY_OFF);
                
                                    //DelayMs(500);
                                    //HB_Relay_Cmd(RELAY_1,RELAY_OFF);
                                    //DelayMs(500);
			}
                           else if(fram_data_buff[0] == CMD_RELAY_DOWN)
                           {
                                    HB_Relay_Cmd(RELAY_1,RELAY_OFF);
                                    HB_Relay_Cmd(RELAY_2,RELAY_ON);
                                    HB_Relay_Cmd(RELAY_4,RELAY_OFF);
                                    //DelayMs(500);
                                    //HB_Relay_Cmd(RELAY_2,RELAY_OFF);
                                    //DelayMs(500);
				
                           }
                           else if(fram_data_buff[0] == CMD_RELAY_STOP)
                           {
                                    HB_Relay_Cmd(RELAY_1,RELAY_OFF);
                                    HB_Relay_Cmd(RELAY_2,RELAY_OFF);
                                    HB_Relay_Cmd(RELAY_4,RELAY_ON);
                                    //DelayMs(500);
                                    //HB_Relay_Cmd(RELAY_3,RELAY_OFF);
                                    //DelayMs(500);
                           }
			
		}


                Floor_CurrentCount += System.Device.Encoder.Enc_GetCount();

        
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

        App.Weight = 0x0;
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
        TIM1->CNT = 10000;
        Floor_CurrentCount = 0;

        System.Device.Systick.Register(Systick1000, SystickRoutine);
      
    

        System.Device.Adc.Register(AdcChannel0, (ushort *)(&App.Weight));
        //System.Device.Adc.Register(AdcChannel1, (ushort *)(&App.Data.Current));

        //System.Device.Usart1.RxdRegister(Usart1RxdFunction);

        System.Device.Usart3.RxdRegister(Usart3RxdFunction);

        //TIM1->CNT = 65523;
}

//AD算术平均值数字滤波
u16 ADC_Filter(void)
{
        u32 sum=0;
        u16 count;
        for (count=0;count<N_ADC;count++)
        {
                sum+=(u16)App.Weight;
                DelayMs(1);
        }
        return (u16)(sum/N_ADC);
}


//中值滤波算法:采集32个，去掉前8个和后8个，取中间的16个值的平均值
u16  ADC_Filter_1(void)
{
        u32 sum=0;
        u16 value_buff[N_ADC];            //定义存储数据的数组
        u16 count,i,j,temp;
        for(count=0;count<N_ADC;count++)  //获取数据
        {
                value_buff[count] = (u16)App.Weight;
                DelayMs(1);
                //delay();                 //如果采集数据比较慢，那么就需要延时或中断
        }

        for(i=0;i<N_ADC;i++)            //用冒泡法对数据进行排序，当然最好用其他排序方法
        {
                for ( j = i; j < N_ADC; j++)
                {
                        if(value_buff[i]>value_buff[j])
                        {
                                temp=value_buff[i];
                                value_buff[i]=value_buff[j];
                                value_buff[j]=temp;
                        }
                }      
        }
         for (count=8;count<N_ADC-8;count++)//取中间16个的和
        {
                sum+=value_buff[count];
        }
         
        return (u16)(sum/(N_ADC-16));
        
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
        
        InitializeApp(); 	//初始化应用层

        HB_LED_State(LED_ERR_16);
        while(1)
        {
                //临时屏蔽，20141212
                //Scan_Input_Value();


                App.Weight_Send = ADC_Filter_1();//这里面大约延时了一段时间32ms

                App.Input_Data = TIM1->CNT;

                //App.Input_Data = Floor_CurrentCount;
                //App.Input_Data = 1;

                //App.Input_Data =TIM_GetCapture1(TIM1);

                //App.Weight_Send = Floor_CurrentCount;
      
                //HB_Send_ErrorAndWeight(App.Input_Data,App.Weight_Send);
                
                HB_Send_ErrorAndWeight(App.Input_Data,Floor_CurrentCount);

                //printf("App.Input_Data===%d\r\n",App.Input_Data);
                //printf("Floor_CurrentCount===%d\r\n",Floor_CurrentCount);
                //DelayMs(1000);
                

        }
                                                        //创建业务逻辑任务，最高优先级0 
    //System.OS.CreateTask(LogicTask, &LogicTaskStack[LogicTaskStackSum - 1]);
                                                        //创建菜单界面任务，最低优先级1
    //System.OS.CreateTask(MenuTask, &MenuTaskStack[MenuTaskStackSum - 1] );
                                                        //创建业务逻辑任务的消息队列
    //LogicTaskMessageQueuePointer = System.OS.CreateMessageQueue(LogicTaskMessageQueue, 40);

    //System.OS.Start();                                                  //RTOS启动
}



