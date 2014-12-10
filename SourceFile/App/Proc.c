

#include "system.h"
#include "app.h"




static void HB_GPIO_Set_Value(u8 HB_Gpio_type,u8 HB_Gpio_Value)
{

        System.Device.IO.HB_Gpio_Set_Value(HB_Gpio_type,HB_Gpio_Value);

}

static u8 HB_GPIO_Get_Value(u8 HB_Gpio_type)
{

        return System.Device.IO.HB_Gpio_Get_Value(HB_Gpio_type);

}

//用4个led灯来显示状态错误，供调试使用。
//正常情况下是四个led灯全亮表示正常
//然后以灯灭的组合来表示2^4种错误代码，这个后续定义
//
//
//
void    HB_LED_State(u8 Led_Error)
{
        switch(Led_Error)
        {
                case    LED_OK:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_FALSE);
                        break;
                case   LED_ERR_1:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_FALSE);
                        break;
                case   LED_ERR_2:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_FALSE);
                        break;
                case   LED_ERR_3:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_FALSE);
                        break ;                  
                case   LED_ERR_4:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_FALSE);
                        break;
                case   LED_ERR_5:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_FALSE);
                        break;
                case   LED_ERR_6:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_FALSE);
                        break;
                case   LED_ERR_7:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_FALSE);
                        break;
                case   LED_ERR_8:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_TRUE);
                        break;
                case   LED_ERR_9:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_TRUE);
                        break;
                case   LED_ERR_10:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_TRUE);
                        break;
                   case   LED_ERR_11:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_TRUE);
                        break;
                            case   LED_ERR_12:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_TRUE);
                        break;
                            case   LED_ERR_13:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_TRUE);
                        break;
                            case   LED_ERR_14:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_FALSE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_TRUE);
                        break;
                            case   LED_ERR_15:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_TRUE);
                        break;






                        
                case   LED_ERR_16:
                        HB_GPIO_Set_Value(LED15,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED16,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED17,LED_VALUE_TRUE);
                        HB_GPIO_Set_Value(LED18,LED_VALUE_TRUE);
                        break;
                
                default:
                        break;
        }

}



//对继电器操作函数，
//参数1:relay==第几个继电器1~6

///2:realy_value===继电器打开或者关闭(RELAY_ON....RELAY_OFF)
//
//
void    HB_Relay_Cmd(u8 Relay,u8 Realy_Value)
{
        switch(Relay)
        {
                case    RELAY_1:
                        HB_GPIO_Set_Value(OUT501,Realy_Value);
                        break;
                case    RELAY_2:
                        HB_GPIO_Set_Value(OUT502,Realy_Value);
                        break;
                case    RELAY_3:
                        HB_GPIO_Set_Value(OUT503,Realy_Value);
                        break;
                case    RELAY_4:
                        HB_GPIO_Set_Value(OUT504,Realy_Value);
                        break;                        
                case    RELAY_5:
                        HB_GPIO_Set_Value(OUT505,Realy_Value);
                        break;
                case    RELAY_6:
                        HB_GPIO_Set_Value(OUT506,Realy_Value);
                        break;                        
                default:
                        break;
        }              

}


//扫描16路输入的值
u16     Scan_Input_Value(void)
{

        u16     ScanValue,ScanValue_1;
        ScanValue = System.Device.IO.Scan_External_Input_Value();
        DelayMs(10);
        ScanValue_1= System.Device.IO.Scan_External_Input_Value();

        if(ScanValue == ScanValue_1)
        {
                App.Input_Data = ScanValue;
        }
        return 0;
}





