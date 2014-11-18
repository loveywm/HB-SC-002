
#include "system.h"
#include "app.h"

#define IdentifyNumberAddress  0x1FFFF7E8

#define LogicTaskStackSum   400             //ҵ���߼�����ջ���
#define MenuTaskStackSum    100             //�˵���������ջ���

uint  LogicTaskStack[LogicTaskStackSum];    //ҵ���߼�����ջ����
uint  MenuTaskStack[MenuTaskStackSum];      //�˵���������ջ����

void * LogicTaskMessageQueue[40];           //ҵ���߼���Ϣ����

AppStruct App;

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
    //�ص�������10msһ�α�ϵͳʱ�ӵ���

        //10ms���һ��16״ֵ̬�����±��浽��Ӧ��ȫ�ֱ�����
        App.Input_Data = Scan_Input_Value();
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
    /*
    ParameterStruct parameter;
    LogStruct log;

    log = System.Device.Storage.Log.Profile();
    printf("\n��־�ܴ洢�ռ� = %d", log.Space);
    printf("\n��־�Ѵ洢�ռ�  = %d", log.Size);
    printf("\n��־��ȡ�� = %x", (uint)(log.ReadPointer));
    printf("\n��־д��� = %x", (uint)(log.WritePointer));
    
    parameter = System.Device.Storage.Parameter.Profile();

    printf("\n\n�����ܴ洢�ռ� = %d", parameter.Space);
    printf("\n�����Ѵ洢�ռ�  = %d", parameter.Size);
    printf("\n�����洢���� = %d\n", parameter.Entries);
    
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

    System.Device.Systick.Register(Systick100, SystickRoutine);

    System.Device.Adc.Register(AdcChannel0, (ushort *)(&App.Data.Voltage));
    System.Device.Adc.Register(AdcChannel1, (ushort *)(&App.Data.Current));

    System.Device.Usart1.RxdRegister(Usart1RxdFunction);

    System.Device.Usart3.RxdRegister(Usart3RxdFunction);
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
        
        InitializeApp();                                                    //��ʼ��Ӧ�ò�



        while(1)
        {

               if(App.Input_Data != Input_invalid)
                {
                        //���ڽ����ֵ���͵������
                        
               } 

                //��adcֵ���͵������


                //������ֵ���͵������


                //���շ��������

        }
                                                        //����ҵ���߼�����������ȼ�0 
    //System.OS.CreateTask(LogicTask, &LogicTaskStack[LogicTaskStackSum - 1]);
                                                        //�����˵���������������ȼ�1
    //System.OS.CreateTask(MenuTask, &MenuTaskStack[MenuTaskStackSum - 1] );
                                                        //����ҵ���߼��������Ϣ����
    //LogicTaskMessageQueuePointer = System.OS.CreateMessageQueue(LogicTaskMessageQueue, 40);

    //System.OS.Start();                                                  //RTOS����
}



