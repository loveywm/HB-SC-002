#ifndef		_UART_H_
#define		_UART_H_

/************************************************************************************************************
通信协议的格式

协议头+命令字+数据长度+数据+校验和+协议尾



**************************************************************************************************************/


typedef unsigned char byte;


enum{

        CMD_RT_DATA=0x01,//实时上传重量和16路接口错误代码
        CMD_LEVELING_DATA,//平层传平层的值

        


        //继电器控制的上，下，停止三个转太
        CMD_RELAY_UP,
        CMD_RELAY_DOWN,
        CMD_RELAY_STOP,

};


#define         CMD_RT_DATA_LEN         6

extern void Usart3RxDataTOApp(byte data);
//extern  unsigned char Rcv_GPRS_Cmd(void);
void HB_Send_ErrorAndWeight(u16 error,u32 weight);
unsigned char   Rcv_Cmd(void);

#endif

