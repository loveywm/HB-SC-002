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
*                                ��ʶ:wangsw
*
*                                QQ:26033613
*                               QQȺ:291235815
*                        �Ա���:http://52edk.taobao.com
*                      ��̳:http://gongkong.eefocus.com/bbs/
*                ����:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*�ļ���   : os.h
*����     : RTOSͷ�ļ�
*����ʱ�� : 2013.7.21
********************************************************************************
*/

#ifndef __OS_H
#define __OS_H

/*���������궨�壬����Ҫ��ʵ�����ף���������쳣*/
#define TaskSum     2           /*��������*/
#define EventSum    1           /*�¼�������*/                            			 
#define QueueSum    1           /*��Ϣ��������*/

typedef struct                  //�������
{
    void    * * Start;          //ָ����п�ʼ
    void    * * End;            //ָ����н���
    void    * * In;             //����һ����Ϣ
    void    * * Out;            //ȡ��һ����Ϣ
    ushort      Size;           //���г���
    ushort      Entries;        //��Ϣ����
} Queue;

typedef struct                  //�����¼����ƿ�
{                                               
    byte 	Type;               //�¼�����
    ushort 	Counter;            //�ź�
    void  * MessagePointer;     //��Ϣָ��
    byte 	PriorityTable;      //�ȴ��������ȼ���
} Event;
  
extern void InitializeOs(void);

#endif






















