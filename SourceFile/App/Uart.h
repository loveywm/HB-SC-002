#ifndef		_UART_H_
#define		_UART_H_

/************************************************************************************************************
ͨ��Э��ĸ�ʽ

Э��ͷ+������+���ݳ���+����+У���+Э��β



**************************************************************************************************************/


typedef unsigned char byte;


enum{

        CMD_RT_DATA=0x01,//ʵʱ�ϴ�������16·�ӿڴ������
        CMD_LEVELING_DATA,//ƽ�㴫ƽ���ֵ


        //�̵������Ƶ��ϣ��£�ֹͣ����ת̫
        CMD_RELAY_UP,
        CMD_RELAY_DOWN,
        CMD_RELAY_STOP,

};




void Usart3RxDataTOApp(byte data);
unsigned char   Rcv_GPRS_Cmd(void);



#endif

