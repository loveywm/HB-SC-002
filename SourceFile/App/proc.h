#ifndef _PROC_H_
#define _PROC_H_



#define         LED_VALUE_TRUE        1
#define         LED_VALUE_FALSE       0

#define         RELAY_ON        1
#define         RELAY_OFF        0

enum{

        //�Ƶ�2������ϱ�ʾ�������
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

        //6·�̵�����־
        RELAY_1,
        RELAY_2,
        RELAY_3,
        RELAY_4,
        RELAY_5,
        RELAY_6,
        
};


void    HB_LED_State(u8 Led_Error);
void    HB_Relay_Cmd(u8 Relay,u8 Realy_Value);

#endif /*PROC_H_*/
