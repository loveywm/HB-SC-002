/*
********************************************************************************
********************************************************************************
*文件名     : device_io.c
*作用       : 输入输出设备
*原理       : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/08/13       初始版本
********************************************************************************
*/

#include "drive.h"
#include "system.h"



#define BeepPin	PaOut(1)



static void SetBeep(bool status)
{
    BeepPin = status;
}

//参数:lednum===第几个led ,取值范围为1~4;              value====led的值,[取值为0或1
static void Set_Led_Value(u8 lednum,u8 value)
{
        
        PcOut(lednum-1) = value; 

}


//relay_num取值为1 2 3 4 5 6
//value 取值为0或1
static void Set_Relay_Value(u8 relay_num,u8 value)
{
        
        if(relay_num<5)
        {
                PaOut(relay_num+3) = value;//1 ///1 2 3 4对应的是PA脚的4 5 6 7，所以要加3
        }
        else
        {
                PcOut(relay_num) = value;////5 6对应的是PC脚的5 6 
        }

}

//输入是状态引脚数字,取值1到16
//
static u8 Get_State_IO_Value(u8 state_num)
{
        if(state_num < 6)
        {
                return PbIn(1);
        }


}


static void HB_Gpio_Set_Value(u8 HB_Gpio_type,u8 HB_Gpio_Value)
{
        switch(HB_Gpio_type)
        {
                //四个led灯
                case    LED15:
                        PcOut(0) = HB_Gpio_Value; 
                        break
                case    LED16:
                        PcOut(1) = HB_Gpio_Value; 
                        break;
                case    LED17:
                        PcOut(2) = HB_Gpio_Value; 
                        break;
                case    LED18:
                        PcOut(3) = HB_Gpio_Value; 
                        break;
                    //六个继电器    
                   case    OUT501:
                        PaOut(4) = HB_Gpio_Value; 
                        break;
                    case    OUT502:
                        PaOut(5) = HB_Gpio_Value; 
                        break;
                   case    OUT503:
                        PaOut(6) = HB_Gpio_Value; 
                        break;
                   case    OUT504:
                        PaOut(7) = HB_Gpio_Value; 
                        break;
                   case    OUT505:
                        PcOut(4) = HB_Gpio_Value; 
                        break;
                   case    OUT506:
                        PcOut(5) = HB_Gpio_Value; 
                        break;
                    default:
                        break;
        }
}


static u8  HB_Gpio_Get_Value(u8 HB_Gpio_type)
{
        u8 value=0;
        switch(HB_Gpio_type)
        {
                case    IN_301:
                        value = PbIn(9);
                        break;
                case    IN_302:
                        value = PbIn(8);
                        break;
                case    IN_303:
                        value = PbIn(7);
                        break;
                case    IN_304:
                        value = PbIn(6);
                        break;
                case    IN_305:
                        value = PbIn(5);
                        break;
                case    IN_306:
                        value = PcIn(12);
                        break;

                        
      case    IN_201:
                        value = PcIn(11);
                        break;
      case    IN_202:
                        value = PcIn(10);
                        break;
      case    IN_203:
                        value = PcIn(9);
                        break;
      case    IN_204:
                        value = PcIn(8);
                        break;
      case    IN_205:
                        value = PcIn(7);
                        break;
      case    IN_206:
                        value = PcIn(6);
                        break;
                                
        case    IN_207:
                        value = PbIn(15);
                        break;                    
        case    IN_208:
                        value = PbIn(14);
                        break;                          
        case    IN_209:
                        value = PbIn(13);
                        break;    
        case    IN_210:
                        value = PbIn(12);
                        break;                             
        default:
                break;
        }
        return value;
}





//扫描16个外部输入状态，检测其值
//正常情况下是高电平，外部有变动时候，就会变为低电平
static u16     Scan_External_Input_Value(void)
{
         u16 scanValue;
	
        scanValue = Input_invalid;
        if(PbIn(9) == 0)
                scanValue &= Input_invalid_1;
        if(PbIn(8) == 0)
                scanValue &= Input_invalid_2;
        if(PbIn(7) == 0)
                scanValue &= Input_invalid_3;
        if(PbIn(6) == 0)
                scanValue &= Input_invalid_4;
        if(PbIn(5) == 0)
                scanValue &= Input_invalid_5;
        if(PcIn(12) == 0)
                scanValue &= Input_invalid_6;

        if(PcIn(11) == 0)
                scanValue &= Input_invalid_7;
        if(PcIn(10) == 0)
                scanValue &= Input_invalid_8;
        if(PcIn(9) == 0)
                scanValue &= Input_invalid_9;
        if(PcIn(8) == 0)
                scanValue &= Input_invalid_10;
        if(PcIn(7) == 0)
                scanValue &= Input_invalid_11;
        if(PcIn(6) == 0)
                scanValue &= Input_invalid_12;

        if(PbIn(15) == 0)
                scanValue &= Input_invalid_13;
        if(PbIn(14) == 0)
                scanValue &= Input_invalid_14;
        if(PbIn(13) == 0)
                scanValue &= Input_invalid_15;
        if(PbIn(12) == 0)
                scanValue &= Input_invalid_16;


        return scanValue;
}


/*******************************************************************************
* 函数名	: InputSystickRoutine
* 描述	    : 系统节拍100/S，即10mS一次扫描获取按键值，在多次检测确认后，
*           : 发送按键的映射消息到LogicTask的消息队列中。
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/09       初始版本
*******************************************************************************/
void InputSystick100Routine(void) 
{
        u16 scanValue;

        scanValue = Scan_External_Input_Value();

        if (scanValue != Input_invalid) 
        {


        }

}


void InitializeIO(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
	
        //GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
        //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        //GPIO_Init(GPIOA, &GPIO_InitStructure);
        //BeepPin = 0;    


        //loveywm 20141114
        //四个LED灯的IO口初始化
        //C0=led1......C1=led2.....C2=led3.....C3=led4
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        PaOut(0) = 0;
        PaOut(1) = 0;
        PaOut(2) = 0;
        PaOut(3) = 0;



        //六路继电器输入输出控制
        //A4===P1
        //A5===P2
        //A6===P3
        //A7===P4
        //C4===P5
        //C5===P6
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        PaOut(4) = 0;
        PaOut(5) = 0;
        PaOut(6) = 0;
        PaOut(7) = 0;
        
        PcOut(4) = 0;
        PcOut(5) = 0;

        //6路输入状态检测
        //IN_301===B9
        //IN_302===B8
        //IN_303===B7
        //IN_304===B6
        //IN_305===B5
        //IN_306===C12
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        //第二个6路输入状态检测
        //IN_201===C11
        //IN_202===C10
        //IN_203===C9
        //IN_204===C8
        //IN_205===C7
        //IN_206===C6
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11|GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        //备用4路输入状态检测
        //IN_207===B15
        //IN_208===B14
        //IN_209===B13
        //IN_210===B12

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

      

        
    
        System.Device.IO.SetBeep = SetBeep;

        System.Device.IO.Set_Led_Value = Set_Led_Value;
        System.Device.IO.Set_Relay_Value = Set_Relay_Value;
        
        System.Device.IO.HB_Gpio_Set_Value = HB_Gpio_Set_Value;
        System.Device.IO.HB_Gpio_Get_Value = HB_Gpio_Get_Value;

        System.Device.IO.Scan_External_Input_Value = Scan_External_Input_Value;
        
}

