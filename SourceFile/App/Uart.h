#ifndef		_UART_H_
#define		_UART_H_

/************************************************************************************************************
ͨ��Э��ĸ�ʽ

Э��ͷ+������+���ݳ���+����+У���+Э��β



**************************************************************************************************************/


typedef unsigned char byte;
///////////////////////////////////֡����
//֡ͷ
#define   PROTOCOL_HEAD_1       0x5A
#define   PROTOCOL_HEAD_1_COD   0xA5
#define   PROTOCOL_HEAD_2       0x55

//֡β
#define   PROTOCOL_TAIL_1       0x6A
#define   PROTOCOL_TAIL_1_COD   0x95
#define   PROTOCOL_TAIL_2       0x69

//ת���
#define   PROTOCOL_ESC_CHAR     0x99
#define   PROTOCOL_ESC_CHAR_COD 0x66

enum{

        CMD_RT_DATA=0x01,//ʵʱ�ϴ�������16·�ӿڴ������
        CMD_LEVELING_DATA,//ƽ�㴫ƽ���ֵ

        


        //�̵������Ƶ��ϣ��£�ֹͣ����ת̫
        CMD_RELAY_UP,
        CMD_RELAY_DOWN,
        CMD_RELAY_STOP,

        //ƽ����صĿ��ƴ���
        CMD_LEVEL_UPDATA_CURRENT_COUNT,//���µ�ǰ����������ֵ
        CMD_LEVEL_UPDATA_FLOOR,//����¥�����ݽṹָ��
        CMD_LEVEL_UPDATA_LAST_COUNT,//������һ�α����ļ�����ֵ
        CMD_LEVEL_MODE_AUTO,//�����Զ�ƽ��ģʽ��
	CMD_LEVEL_RETURN_FLAG,//����ƽ����ֵ
        

};


#define         CMD_RT_DATA_LEN         6
#define         CMD_CURRENT_COUNT_DATA_LEN         4

extern void Usart3RxDataTOApp(byte data);
//extern  unsigned char Rcv_GPRS_Cmd(void);
void HB_Send_ErrorAndWeight(u16 error,u32 weight);
unsigned char   Rcv_Cmd(void);



void HB_Send_Current_Count(u32 count);
void HB_Send_Floor(Floor_Data   *floor);
void HB_Send_Last_Count(u32 count);
void HB_Send_Level_Return(u8 ret);

#endif

