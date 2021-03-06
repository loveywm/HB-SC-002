
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
u8  HB_Run_Flag =0;//升降机工作模式，=1是自动平层模式，=0是手动模式
u8  HB_Run_Floor = 1;//升降机运行的目标楼层，主要是接受控制端发来的数据
u8  HB_Level_Flag = 0;//平层上升下降的标志=====上升1，下降2，停止0


//关于自动平层的数值使用
signed int Floor_CurrentCount = 0;//用于计数编码器的的值 ，要在开机时自动和控制端同步
Floor_Data   floor_tmp[MAX_FLOOR_NUM];
/////////////////////////////////////////////



void HB_Run_Mode(void)
{
    //signed int count_tmp;
    if(HB_Run_Flag == 1)
    {
           //count_tmp = (signed int )floor_tmp[HB_Run_Floor-1].floor_count;

            if(Floor_CurrentCount > (signed int )floor_tmp[HB_Run_Floor-1].floor_count)//向下运行
            {
                    
                     HB_Relay_Cmd(RELAY_1,RELAY_OFF);
                     HB_Relay_Cmd(RELAY_2,RELAY_ON);
                     HB_Relay_Cmd(RELAY_4,RELAY_OFF);

                 
            }
            else if(Floor_CurrentCount < (signed int )floor_tmp[HB_Run_Floor-1].floor_count)//向上运行
            {
                    HB_Relay_Cmd(RELAY_1,RELAY_ON);
                    HB_Relay_Cmd(RELAY_2,RELAY_OFF);
                    HB_Relay_Cmd(RELAY_4,RELAY_OFF);
                   
                
            }
            else if(Floor_CurrentCount == (signed int )floor_tmp[HB_Run_Floor-1].floor_count)//停止
            {
                 HB_Relay_Cmd(RELAY_1,RELAY_OFF);
                HB_Relay_Cmd(RELAY_2,RELAY_OFF);
                HB_Relay_Cmd(RELAY_4,RELAY_ON);
                
                HB_Run_Flag = 0;
                HB_Send_Level_Return(1);//1//表示OK
        
           }

    }
    else
    {


    }
    


}




//判断报文是否校验和正确,pRep保存的报文是掐头去尾
char   Cmd_Rep_valid(char*  pRep,char* plen)
{
	char i = 0,j = 0;
	//char sum = PROTOCOL_HEAD_1 + PROTOCOL_HEAD_2;
	char sum = 0;
	////分析报文，将转义符处理掉
    	for(i=0;i<(*plen-1);)//最后一个是校验和，不用判断
	{
		sum += pRep[i];
		if(pRep[i] == PROTOCOL_ESC_CHAR)
		{
			if(i == *plen-2)
			{
				return 0;
			}
			else
			{
				if((pRep[i+1]!= PROTOCOL_ESC_CHAR_COD)&&
					(pRep[i+1]!= PROTOCOL_TAIL_1_COD)&&
					(pRep[i+1]!= PROTOCOL_HEAD_1_COD))
					return 0;
				else
				{
					sum += pRep[i+1];
					fram_data_buff[j] = 0xff - pRep[i+1];
					j++;
					i+=2;
				}
			}
		}
		else
		{
			fram_data_buff[j] = pRep[i];
			j++;
			i++;
		}
	}
	fram_data_buff[j] = pRep[*plen-1];
	///判断校验和
	if(sum != fram_data_buff[j])
		return 0;
	*plen = fram_data_buff[1]+3;//头+数据长度+数据+校验和
    	return 1;
}

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
                        if(Cmd_Rep_valid(fram_data_buff,&fram_len))//处理接受的数据是否包含和协议头尾相同的字符，修改数据长度
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

                            else if(fram_data_buff[0] == CMD_LEVEL_UPDATA_CURRENT_COUNT)
			 {

                                    Floor_CurrentCount |= fram_data_buff[5]<<24;
				Floor_CurrentCount |= fram_data_buff[4]<<16;
				Floor_CurrentCount |= fram_data_buff[3]<<8;
				Floor_CurrentCount |= fram_data_buff[2];

                                    HB_LED_State(LED_ERR_1);
                            }
                           else if(fram_data_buff[0] == CMD_LEVEL_UPDATA_FLOOR)
			 {
                                    u8	floor_num_tmp = 0;
				floor_num_tmp = fram_data_buff[3];//楼层数位置
				floor_tmp[floor_num_tmp-1].floor_num = floor_num_tmp;
                
				floor_tmp[floor_num_tmp-1].floor_count  |= fram_data_buff[7]<<24;
				floor_tmp[floor_num_tmp-1].floor_count  |= fram_data_buff[6]<<16;
				floor_tmp[floor_num_tmp-1].floor_count  |= fram_data_buff[5]<<8;
				floor_tmp[floor_num_tmp-1].floor_count  |= fram_data_buff[4];

                                    HB_LED_State(LED_ERR_2);
                                
                            }
                           else if(fram_data_buff[0] == CMD_LEVEL_MODE_AUTO)
                            {

                                   HB_Run_Flag = 1;

                                    HB_Run_Floor = fram_data_buff[2];
                           }
                        }	
		}


                Floor_CurrentCount += System.Device.Encoder.Enc_GetCount();
                HB_Run_Mode();

        
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
        //Floor_CurrentCount = 9999;


        
        System.Device.Usart3.RxdRegister(Usart3RxdFunction);

        HB_LED_State(LED_OK);

        //接收服务端的平层计数值，这个初始化有
        while(1)
        {
                if(Rcv_Cmd() == 1)
                {
                        if(Cmd_Rep_valid(fram_data_buff,&fram_len))//处理接受的数据是否包含和协议头尾相同的字符，修改数据长度
                        {
                                if(fram_data_buff[0] == CMD_LEVEL_UPDATA_LAST_COUNT)
			     {
                                             Floor_CurrentCount |= fram_data_buff[5]<<24;
					Floor_CurrentCount |= fram_data_buff[4]<<16;
					Floor_CurrentCount |= fram_data_buff[3]<<8;
					Floor_CurrentCount |= fram_data_buff[2];

                                            Floor_CurrentCount   -= 10001;

                                            Floor_CurrentCount += System.Device.Encoder.Enc_GetCount();

                                              break;
                                }
                                else if(fram_data_buff[0] == CMD_LEVEL_UPDATA_FLOOR)
			     {
                                    u8	floor_num_tmp = 0;
				floor_num_tmp = fram_data_buff[3];//楼层数位置
				floor_tmp[floor_num_tmp-1].floor_num = floor_num_tmp;
                
				floor_tmp[floor_num_tmp-1].floor_count  |= fram_data_buff[7]<<24;
				floor_tmp[floor_num_tmp-1].floor_count  |= fram_data_buff[6]<<16;
				floor_tmp[floor_num_tmp-1].floor_count  |= fram_data_buff[5]<<8;
				floor_tmp[floor_num_tmp-1].floor_count  |= fram_data_buff[4];

                                    //printf("floor_tmp[floor_num_tmp-1].floor_count==%d\r\n",floor_tmp[floor_num_tmp-1].floor_count);

                                    HB_LED_State(LED_ERR_2);
                                
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
                        
                }
        }

        
        System.Device.Systick.Register(Systick1000, SystickRoutine);
      
    

        System.Device.Adc.Register(AdcChannel0, (ushort *)(&App.Weight));
        //System.Device.Adc.Register(AdcChannel1, (ushort *)(&App.Data.Current));

        //System.Device.Usart1.RxdRegister(Usart1RxdFunction);

        

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
      
        //int ywm;
        System.Initialize();                                                //初始化系统层
        
        InitializeApp(); 	//初始化应用层

        HB_LED_State(LED_ERR_16);
        while(1)
        {
                //临时屏蔽，20141212
                //Scan_Input_Value();


                App.Weight_Send = ADC_Filter_1();//这里面大约延时了一段时间32ms

                App.Input_Data = TIM1->CNT;
      
                HB_Send_ErrorAndWeight(App.Input_Data,App.Weight_Send);
                
                //HB_Send_ErrorAndWeight(App.Input_Data,Floor_CurrentCount);

                HB_Send_Current_Count(Floor_CurrentCount);



                //HB_Run_Mode();

             
                

        }
                                                        //创建业务逻辑任务，最高优先级0 
    //System.OS.CreateTask(LogicTask, &LogicTaskStack[LogicTaskStackSum - 1]);
                                                        //创建菜单界面任务，最低优先级1
    //System.OS.CreateTask(MenuTask, &MenuTaskStack[MenuTaskStackSum - 1] );
                                                        //创建业务逻辑任务的消息队列
    //LogicTaskMessageQueuePointer = System.OS.CreateMessageQueue(LogicTaskMessageQueue, 40);

    //System.OS.Start();                                                  //RTOS启动
}



