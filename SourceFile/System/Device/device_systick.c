/*
********************************************************************************
*                            ********
*�ļ���     : device_systick.c
*����       : ϵͳ����
*ԭ��       : 
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/12/16       ��ʼ�汾
********************************************************************************
*/



#include "drive.h"
#include "system.h"

extern void KeySystick100Routine(void);

extern void RtcSystick100Routine(void);

extern void TimerSystick1000Routine(void);

extern void UsartSystick1000Routine(void);
extern void Usart3Systick1000Routine(void);

extern void AdcSystick10000Routine(void);

extern void SysTick_Handler(void);


#define Systick10000Sum 2
#define Systick1000Sum  4
#define Systick100Sum   8

static void Dummy(void) {};

static function Systick10000RegisterPointerBlock[Systick10000Sum] =
{
    Dummy, Dummy
};

static function Systick1000RegisterPointerBlock[Systick1000Sum] =
{
    Dummy, Dummy, Dummy, Dummy
};

static function Systick100RegisterPointerBlock[Systick100Sum] =
{
    Dummy, Dummy, Dummy, Dummy, Dummy, Dummy, Dummy, Dummy
};

/*******************************************************************************
* ������	: RegisterSystick
* ����	    : ϵͳ����ע�ắ��������ע��ص���������ͬ�ٶȵ�ϵͳ���������С�
* �������  : systemTick ϵͳ�����ٶ����� registerFunction ע��ĺ���ָ��
* ���ز���  : bool���������������Ƿ�ɹ�
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
*******************************************************************************/
static bool RegisterSystick(SystickEnum systick, function registerFunction)
{
    static byte Systick100Counter = 0;
    static byte Systick1000Counter = 0;
    static byte Systick10000Counter = 0;

    Assert(registerFunction != (function)0);
    EnterCritical();
    switch(systick)
    {
        case Systick100:
            if (Systick100Counter == Systick100Sum)
                return(false);
            
            Systick100RegisterPointerBlock[Systick100Counter++] = registerFunction;  
            ExitCritical();
            return(true);
        case Systick1000:
            if (Systick1000Counter == Systick1000Sum)
                return(false);
            
            Systick1000RegisterPointerBlock[Systick1000Counter++] = registerFunction;   
            ExitCritical();
            return(true);
        case Systick10000:
            if (Systick10000Counter == Systick10000Sum)
                return(false);
            
            Systick10000RegisterPointerBlock[Systick10000Counter++] = registerFunction;   
            ExitCritical();
            return(true);
        default:
            ExitCritical();
            return(false);
    }
}

void InitializeSystick(void)
{
    System.Device.Systick.Register = RegisterSystick;
    SysTick_Config(SystemCoreClock / 10000);
}



/*******************************************************************************
* ������	: SysTick_Handler
* ����	    : ϵͳ�����ж���ڣ��ǳ���Ҫ��ÿ��10000�Σ���0.1mSһ��
*           : ϵͳ����ͨ��switchϸ��Ϊÿ��1000�Σ�ÿ��100�ε����н���
*           : ͬ��ֱ�ӵ��ã��߲����ע�����
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/07       ��ʼ�汾
*******************************************************************************/
void SysTick_Handler(void)
{
    static unsigned char Counter = 0;
	
    if(++Counter == 100)
    {
        Counter = 0;
    }

    Systick10000RegisterPointerBlock[0]();
    Systick10000RegisterPointerBlock[1]();

    switch(Counter) 
    {
        case 0:
        case 10:
        case 20:
        case 30:
        case 40:
        case 50:
        case 60:
        case 70:
        case 80:
        case 90:
            Systick1000RegisterPointerBlock[0]();
            break;
        case 2:
        case 12:
        case 22:
        case 32:
        case 42:
        case 52:
        case 62:
        case 72:
        case 82:
        case 92:
            Systick1000RegisterPointerBlock[1]();
            break;
        case 4:
        case 14:
        case 24:
        case 34:
        case 44:
        case 54:
        case 64:
        case 74:
        case 84:
        case 94:
            Systick1000RegisterPointerBlock[2]();
            break;
        case 6:
        case 16:
        case 26:
        case 36:
        case 46:
        case 56:
        case 66:
        case 76:
        case 86:
        case 96:
            Systick1000RegisterPointerBlock[3]();
            break;
        case 8:
        case 18:
        case 28:
        case 38:
        case 48:
        case 58:
        case 68:
        case 78:
        case 88:
        case 98:
            UsartSystick1000Routine();
            Usart3Systick1000Routine();
            //TimerSystick1000Routine();
            break;

        case 1:
            Systick100RegisterPointerBlock[0]();
            break;
        case 3:
            Systick100RegisterPointerBlock[1]();
			break;
        case 5:
            Systick100RegisterPointerBlock[2]();
            break;
        case 7:
            Systick100RegisterPointerBlock[3]();
            break;
        case 9:
            Systick100RegisterPointerBlock[4]();
            break;
        case 11:
            Systick100RegisterPointerBlock[5]();
            break;
        case 13:
            Systick100RegisterPointerBlock[6]();
            break;
        case 15:
            Systick100RegisterPointerBlock[7]();
            break;  
            
        case 97:
            //RtcSystick100Routine();
            break;
        case 99:
            //KeySystick100Routine();
            break; 
        default:
            break;
    }

    AdcSystick10000Routine();
}





