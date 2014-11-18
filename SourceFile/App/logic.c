/*
********************************************************************************
*                                Ƕ��ʽ΢ϵͳ
*                                   msOS
*
*                            Ӳ��ƽ̨:msPLC DEMO
*                          ��оƬ:STM32F103R8T6/RBT6
*                           ��������οƼ����޹�˾
*
*                                ����:����ΰ
*                                ����:������
*                                ��ʶ:Wangsw
*
*                                QQ:26033613
*                               QQȺ:291235815
*                        �Ա���:http://52edk.taobao.com
*                      ��̳:http://gongkong.eefocus.com/bbs/
*                ����:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*�ļ���     : logic.c
*����       : ҵ���߼������ļ�
*ԭ��       : ���������Ϣ���簴�����������ȣ���׼��һЩ�����Ĵ���ģʽ
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
********************************************************************************
*/


#include "system.h"
#include "app.h"



/*******************************************************************************
* ������	: FormFocusLongKeyProcess
* ����	    : FormFocusLongKey����ҳ�潹���л���������л�����ҳ��
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
*******************************************************************************/
static void FormFocusLongKeyProcess(void)
{    
   // if (App.Menu.FocusFormPointer == &App.Menu.ServiceForm)
   //     App.Menu.FocusFormPointer = &App.Menu.WorkForm;
  //  else
       // App.Menu.FocusFormPointer ++;

}

/*******************************************************************************
* ������	: SaveKeyProcess
* ����	    : �洢�����ı��ؼ�������ֵ
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
*******************************************************************************/
void SaveKeyProcess(void)
{
//    if (App.Menu.FocusFormPointer->FocusTextBoxDataPointer == null) return;
   
//    System.Device.Storage.Parameter.Write((uint *)App.Menu.FocusFormPointer->FocusTextBoxDataPointer);
}


/*******************************************************************************
* ������	: SaveLongKeyProcess
* ����	    : �洢���в���
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
*******************************************************************************/
void SaveLongKeyProcess(void)
{
    

}


/*******************************************************************************
* ������	: StopKeyDefaultProcess
* ����	    : StopKeyȱʡ������
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
*******************************************************************************/
static void StopKeyDefaultProcess(void)
{
 //   App.Menu.FocusFormPointer->FocusTextBoxDataPointer = null;
//    App.Menu.FocusFormPointer = &App.Menu.WorkForm;
}

static byte Counter = 0;
/*******************************************************************************
* ������	: KeyProcess
* ����	    : ������Ϣ������
* �������  : key�����ⰴ��ֵ
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
*******************************************************************************/
void KeyProcess(KeyEnum key)
{
    //printf("\nKey: = %x", key);
    switch(key)
    {
        case FormFocusLongKey:  //ҳ�泤�����л�
            FormFocusLongKeyProcess();
            break;   
        case TextBoxFocusKey:   //�ı��ؼ�����л�
//            System.Gui.Form.SwitchTextBoxFocus();
            break;
        case AddKey:
        case AddLongKey:
        case SubKey:
        case SubLongKey:
        case AssistUpKey:
        case AssistUpLongKey:
        case AssistDownKey:
        case AssistDownLongKey: //���ְ�������
//            System.Gui.Form.SetTextBoxData(key);
            break;
        case SaveKey:           //�����ݴ洢
            SaveKeyProcess();     
            break;
        case SaveLongKey:       //ȫ�����ݴ洢
            SaveLongKeyProcess();
            break;
        case StartKey:          //��ʼ����,������־д
//            System.Device.Storage.Log.Write("%d\n", Counter++);
            break;
        case StartLongKey:      //��ʼ����,������־��
            //printf(System.Device.Storage.Log.Read(-1));
            break;
        case StopKey:           //ֹͣ����
            StopKeyDefaultProcess();
            break;         
    }
}

/*******************************************************************************
* ������	: LogicTask
* ����	    : �߼�ҵ�����񣬸�����Ϣ�簴������������Ϣ�������
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/03       ��ʼ�汾
*******************************************************************************/
void LogicTask(void)
{
    byte i;
    uint message;
    uint messageData; 
        
    System.OS.DelayTimeTick(2000);//2000

    //��Ƶ��Ӧ�����豸ɨƵ�Լ죬��ȡг���
//    App.Menu.FocusFormPointer = &App.Menu.CheckForm;   
    for(i = 0; i < 16; i++)
    {
        System.OS.DelayTimeTick(100);//100
//        App.Menu.CheckForm.ChartPointer->Column[i] = i % 5;
    }
    //App.Menu.FocusFormPointer = &App.Menu.WorkForm;   

    //�߼�ҵ�������ȡ��Ϣ�����䴦��
    while(true)
    {     
        message = (uint)System.OS.PendMessageQueue(LogicTaskMessageQueuePointer, 0);
        
        messageData = GetMessageData(message);
        switch(GetMessageType(message))
        {
            case KeyMessageType:                    //������Ϣ
                KeyProcess((KeyEnum)messageData);
                break;
            default:                                //�����ʱ����Ϣ
                Function(message);
                break;
        }
    }
}

