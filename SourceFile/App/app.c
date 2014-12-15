
#include "system.h"
#include "app.h"

#include "stm32f10x_gpio.h"


#define N_ADC   32


#define IdentifyNumberAddress  0x1FFFF7E8

#define LogicTaskStackSum   400             //ҵ���߼�����ջ���
#define MenuTaskStackSum    100             //�˵���������ջ���

uint  LogicTaskStack[LogicTaskStackSum];    //ҵ���߼�����ջ����
uint  MenuTaskStack[MenuTaskStackSum];      //�˵���������ջ����

void * LogicTaskMessageQueue[40];           //ҵ���߼���Ϣ����

AppStruct App;

unsigned char  fram_data_buff[MAX_FRAM_LEN];////������鱣��õ���һ����ͷȥβ�ı���֡
unsigned char  fram_len = 0;
//u8 weight_tmp_flag = 0;//��ʱ���������Ƿ���ȵļ�����־
u8  RT_Send_Flag = 0;//ʵʱ�����Ƿ��͵ı�־


signed int Floor_CurrentCount = 0;//���ڼ����������ĵ�ֵ 


/*******************************************************************************
* ������	: SystemTickRoutine
* ����	    : Ӧ�ò�ϵͳ���ģ�����ִ�й̶�������ܣ����ʱ��ȡ����ע�ắ��
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/03       ��ʼ�汾
*******************************************************************************/
static void SystickRoutine(void) 
{
    //�ص�������1msһ�α�ϵͳʱ�ӵ���

        //10ms���һ��16״ֵ̬�����±��浽��Ӧ��ȫ�ֱ�����
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
* ������	: Usart1RxdFunction
* ����	    : Usart1���մ���ص���������Ҫͨ��ע�����ʹ��
* �������  : data Usart����ֵ
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/13       ��ʼ�汾
*******************************************************************************/
static void Usart1RxdFunction(byte data)
{
    PostMessageToLogicTask(KeyMessageType, data);  
}


static void Usart3RxdFunction(byte data)
{
        //PostMessageToLogicTask(KeyMessageType, data); 
        //��data�е�ֵ����Ӧ�ò�
        Usart3RxDataTOApp(data);
}

/*******************************************************************************
* ������	: InitializeData
* ����	    : ��ʼ��Ĭ�ϲ���
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/08       ��ʼ�汾
*******************************************************************************/
void InitializeData(void)
{

        App.Weight = 0x0;
}

/*******************************************************************************
* ������	: InitializeApp
* ����	    : ��ʼ��App
*           : InitializeMmi:��ʼ������
*           : System.Device.SystemTick.Register:ע��Ӧ�ò����
*           : System.Device.Adc.Register:ע��ADC�����豸����
*           : System.Device.Usart1.RxdRegister Usart1���պ���ע��
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/03       ��ʼ�汾
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

//AD����ƽ��ֵ�����˲�
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


//��ֵ�˲��㷨:�ɼ�32����ȥ��ǰ8���ͺ�8����ȡ�м��16��ֵ��ƽ��ֵ
u16  ADC_Filter_1(void)
{
        u32 sum=0;
        u16 value_buff[N_ADC];            //����洢���ݵ�����
        u16 count,i,j,temp;
        for(count=0;count<N_ADC;count++)  //��ȡ����
        {
                value_buff[count] = (u16)App.Weight;
                DelayMs(1);
                //delay();                 //����ɼ����ݱȽ�������ô����Ҫ��ʱ���ж�
        }

        for(i=0;i<N_ADC;i++)            //��ð�ݷ������ݽ������򣬵�Ȼ������������򷽷�
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
         for (count=8;count<N_ADC-8;count++)//ȡ�м�16���ĺ�
        {
                sum+=value_buff[count];
        }
         
        return (u16)(sum/(N_ADC-16));
        
}


/*******************************************************************************
* ������	: main
* ����	    : msOS�����������
*           : InitializeData:��ʼ���洢������
*           : System.Initialize:System��ʼ����ʵ�ֵײ�ϵͳ��ʼ�� 
*           : �����ʼ�����ȵģ����ȼ�Ҳ���ȣ�TaskSumҪ��������
*           : LogicTask: ҵ���߼����񣬳�ʼ��Ϊ������ȼ� 
*           : MenuTask: �������񣬳�ʼ��Ϊ������ȼ�,ע�⣬������Ϊ������ȼ�
*           : LogicTaskMessageQueuePoint:��ʼ��һ���߼�������Ϣ����
* �������  : ��
* ���ز���  : int
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/03       ��ʼ�汾
*******************************************************************************/
int main(void) 
{            
      
        
        System.Initialize();                                                //��ʼ��ϵͳ��
        
        InitializeApp(); 	//��ʼ��Ӧ�ò�

        HB_LED_State(LED_ERR_16);
        while(1)
        {
                //��ʱ���Σ�20141212
                //Scan_Input_Value();


                App.Weight_Send = ADC_Filter_1();//�������Լ��ʱ��һ��ʱ��32ms

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
                                                        //����ҵ���߼�����������ȼ�0 
    //System.OS.CreateTask(LogicTask, &LogicTaskStack[LogicTaskStackSum - 1]);
                                                        //�����˵���������������ȼ�1
    //System.OS.CreateTask(MenuTask, &MenuTaskStack[MenuTaskStackSum - 1] );
                                                        //����ҵ���߼��������Ϣ����
    //LogicTaskMessageQueuePointer = System.OS.CreateMessageQueue(LogicTaskMessageQueue, 40);

    //System.OS.Start();                                                  //RTOS����
}



