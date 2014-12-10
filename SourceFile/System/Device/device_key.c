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
*�ļ���     : device_key.c
*����       : �����豸
*ԭ��       : ����SystemTick 100/�� 10mSɨ���ȡ����ֵ������ֵ��Ϊ������̰�����
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
********************************************************************************
*/



#include "drive.h"
#include "system.h"


#define ScanPin00	PbOut(4)
#define ScanPin01	PbOut(5)

#define ScanPin10	PcIn(10)
#define ScanPin11	PcIn(11)
#define ScanPin12	PcIn(12)
#define ScanPin13	PbIn(3)

#define ShortInterval   3		//�̰�������������õ���3
#define LongInterval    40		//�����������
#define DoubleHitInterval  10		//������˫���󶯼��
#define KeyBeepInterval  20

static byte ScanValueSave = 0xFF;
static byte ScanCounter = 0;
static byte DoubleHitCounter = 0;
static byte KeyBeepCounter = 0;



static byte RemapKey(byte scanValue) 
{
    switch(scanValue)
    {
        case 0xEF:
            return(0);
        case 0xDF:
            return(1);
        case 0xBF:
            return(2);
        case 0x7F:
            return(3);
        case 0xFE:
            return(4);
        case 0xFD:
            return(5);
        case 0xFB:
            return(6);
        case 0xF7:
            return(7);
        default:
            return(invalid);
    }
}

static byte RemapLongKey(byte scanValue) 
{
    switch(scanValue)
    {
        case 0xEF:
            return(0x30);
        case 0xDF:
            return(0x31);
        case 0xBF:
            return(0x32);
        case 0x7F:
            return(0x33);
        case 0xFE:
            return(0x34);
        case 0xFD:
            return(0x35);
        case 0xFB:
            return(0x36);
        case 0xF7:
            return(0x37);
        default:
            return(invalid);
    }	
}

static byte ScanPin(void)
{
    byte scanValue;
	
    scanValue = invalid;
    if(ScanPin13 == 0)
        scanValue &= 0x7F;

    if(ScanPin12 == 0)
        scanValue &= 0xBF;

    if(ScanPin11 == 0)
        scanValue &= 0xDF;

    if(ScanPin10 == 0)
        scanValue &= 0xEF;

    ScanPin00 = 0;
    ScanPin01 = 1;
	

    DelayUs(1);

    if(ScanPin13 == 0)
        scanValue &= 0xF7;

    if(ScanPin12 == 0)
        scanValue &= 0xFB;

    if(ScanPin11 == 0)
        scanValue &= 0xFD;

    if(ScanPin10 == 0)
        scanValue &= 0xFE;

    ScanPin01 = 0;
    ScanPin00 = 1;

    return(scanValue);
}


/*******************************************************************************
* ������	: KeySystickRoutine
* ����	    : ϵͳ����100/S����10mSһ��ɨ���ȡ����ֵ���ڶ�μ��ȷ�Ϻ�
*           : ���Ͱ�����ӳ����Ϣ��LogicTask����Ϣ�����С�
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/09       ��ʼ�汾
*******************************************************************************/
void KeySystick100Routine(void) 
{
    byte scanValue;

//    if (KeyBeepCounter == 1) 
//        System.Device.IO.SetBeep(false);
    
    if (KeyBeepCounter > 0) KeyBeepCounter--;

    scanValue = ScanPin();
	
    if (scanValue == invalid) 
	{
        if(DoubleHitCounter)
        {
            DoubleHitCounter--;
            ScanCounter = 0;
            ScanValueSave = invalid;
            return;
        }
        
        if(ScanValueSave == invalid)
        {
            ScanCounter = 0;
            return;
        }
        
        if (ScanCounter == LongInterval) 
        {
            PostMessageToLogicTask(KeyMessageType, RemapLongKey(ScanValueSave));  
//            System.Device.IO.SetBeep(true);
            KeyBeepCounter = KeyBeepInterval;
            DoubleHitCounter = DoubleHitInterval;
        } 
        else if (ScanCounter > ShortInterval) 
        {
            PostMessageToLogicTask(KeyMessageType, RemapKey(ScanValueSave));
//            System.Device.IO.SetBeep(true);
            KeyBeepCounter = KeyBeepInterval;
            DoubleHitCounter = DoubleHitInterval;
        }
        ScanCounter = 0;
        ScanValueSave = invalid;
	} 
    else 
    {
        ScanCounter++;
		
        if (ScanCounter == 1) 
        {
            ScanValueSave = scanValue;
        } 
        else if (ScanCounter == 2) 
        {
            if (scanValue != ScanValueSave)
            {
                ScanValueSave = invalid;
                ScanCounter = 0;
            }
        }
        else if (ScanCounter >= LongInterval) 
        {
            ScanCounter = LongInterval;
        }
    }
}

void InitializeKey(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);

	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);



	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ScanPin01 = 0;
	ScanPin00 = 1;

}

