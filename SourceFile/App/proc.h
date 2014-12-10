#ifndef _PROC_H_
#define _PROC_H_

//#include "app.h"

#define         LED_VALUE_TRUE        1
#define         LED_VALUE_FALSE       0

#define         RELAY_ON        1
#define         RELAY_OFF        0

enum{

        //灯的2进制组合表示错误代码
        LED_OK,
        LED_ERR_1,        
        LED_ERR_2,        
        LED_ERR_3,        
        LED_ERR_4,        
        LED_ERR_5,        
        LED_ERR_6,        
        LED_ERR_7,        
        LED_ERR_8,        
        LED_ERR_9,        
        LED_ERR_10,        
        LED_ERR_11,      

        LED_ERR_12,
        LED_ERR_13,
        LED_ERR_14,
        LED_ERR_15,
       
        
        LED_ERR_16,

        //6路继电器标志
        RELAY_1,
        RELAY_2,
        RELAY_3,
        RELAY_4,
        RELAY_5,
        RELAY_6,
        
};


extern void    HB_LED_State(u8 Led_Error);
extern void    HB_Relay_Cmd(u8 Relay,u8 Realy_Value);
extern u16     Scan_Input_Value(void);

#endif /*PROC_H_*/
