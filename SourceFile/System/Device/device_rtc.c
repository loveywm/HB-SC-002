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
*�ļ���     : device_rtc.c
*����       : ����ʱ���豸�����32.768KHz���������ʱ�ӣ�һ�����ھ��Ȳ��߳���
*ԭ��       : ����ϵͳ���Ĳ�����ʱ�ӷ�Ƶ��õ�ʱ���֡��룬�˾��Ȳ��ߣ�����һ�㳡��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
********************************************************************************
*/

#include "system.h"



/*******************************************************************************
* ������	: RtcSystickRoutine
* ����	    : Rtcϵͳʱ��100/S����10mSһ�ε���
* �������  : 
* ���ز���  : 
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/10       ��ʼ�汾
*******************************************************************************/
void RtcSystick100Routine(void) 
{
    static byte Counter = 0;
    
    if(++Counter == 100)
    {
        Counter = 0;

        if (System.Device.Rtc.Second < 59)
            System.Device.Rtc.Second++;
        else
        {
            EnterCritical();
            System.Device.Rtc.Second = 0;
            if(System.Device.Rtc.Minute < 59)
                System.Device.Rtc.Minute++;
            else
            {
                System.Device.Rtc.Minute = 0;
                if(System.Device.Rtc.Hour < 23)
                    System.Device.Rtc.Hour++;
                else
                {
                    System.Device.Rtc.Hour = 0;
                    System.Device.Rtc.Day++;
                } 
            }
            ExitCritical();
        }
    }	
}

void InitializeRtc(void) 
{
        System.Device.Rtc.Second = 0;
        System.Device.Rtc.Minute = 0;
        System.Device.Rtc.Hour = 0;
        System.Device.Rtc.Day = 0;
}

