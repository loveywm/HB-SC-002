#include "system.h"
#include "app.h"


//����ѭ�����н��ܷ�ʽ
#define UART3_RX_BUFFER_SIZE  256
#define UART3_RX_BUFFER_MASK  255
static unsigned char UART3_RxBuf[UART3_RX_BUFFER_SIZE];
static volatile unsigned char UART3_RxHead = 0;
static volatile unsigned char UART3_RxTail = 0;

//����������ã��������Ǵ��ڽ����жϺ󴫵�ֵ������data
void Usart3RxDataTOApp(byte data)
{
	UART3_RxHead = ( UART3_RxHead + 1 ) & UART3_RX_BUFFER_MASK;
	if( UART3_RxHead == UART3_RxTail )
	{
		/* ERROR! Receive buffer overflow */
	}
	UART3_RxBuf[UART3_RxHead] = data; /* store received data in buffer */
}

unsigned char ReceiveByte3(char* pdata)
{

	if(UART3_RxHead == UART3_RxTail)
	    return 0;

	UART3_RxTail = ( UART3_RxTail + 1 )&UART3_RX_BUFFER_MASK;/* calculate buffer index and store new index*/
	*pdata = UART3_RxBuf[UART3_RxTail];
	return 1;

}
void  FlushUart3(void)
{
    	UART3_RxTail = UART3_RxHead;//UART1_RxTail = UART1_RxHead;
}

///////////////////////////////////֡����
//֡ͷ
#define   PROTOCOL_HEAD_1       0x5A
#define   PROTOCOL_HEAD_1_COD   0xA5
#define   PROTOCOL_HEAD_2       0x55

//֡β
#define   PROTOCOL_TAIL_1       0x6A
#define   PROTOCOL_TAIL_1_COD   0x95
#define   PROTOCOL_TAIL_2       0x69

/////////////////////////////////////////////
#define    R_DATA_BUFF_LEN  256    //֮����Ҫ����256�����ȵ����飬����Ϊ���ܹ������顰��β��ӡ�����Ϊ0 -1 = 255 �� 255+1 = 0��
#define    R_DATA_BUFF_MASK 255
unsigned char rmote_buff[R_DATA_BUFF_LEN];
unsigned int rmote_head_index = 0;//�洢���ݵ�ָ��
unsigned int rmote_rep_index  = R_DATA_BUFF_LEN + 1;//ͷλ��
unsigned char is_find_head = 0;

unsigned char   Rcv_GPRS_Cmd(void)
{
    	int  i = 0;
	rmote_head_index = 0;
    	while(ReceiveByte3(&rmote_buff[rmote_head_index]))
  	{
		//��һ��Ѱ��ͷ
		if((rmote_rep_index == R_DATA_BUFF_LEN+1)&&(rmote_buff[rmote_head_index] == PROTOCOL_HEAD_1))//�ҵ�ͷ��
		{ 
		 	rmote_rep_index = rmote_head_index;
			printf("111rmote_head_index==%d\r\n",rmote_head_index);
			printf("111rmote_rep_index==%d\r\n",rmote_rep_index);
		}
		
		//�ڶ�����ͷ
		if((rmote_rep_index != rmote_head_index)&&(rmote_rep_index != (R_DATA_BUFF_LEN+1)))
		{
		    	if(is_find_head == 0)
		    	{
				printf("222is_find_head==%d\r\n",is_find_head);
				if(rmote_buff[(rmote_rep_index+1)&R_DATA_BUFF_MASK] == PROTOCOL_HEAD_2)
			    		is_find_head = 1;
			    	else
					rmote_rep_index = R_DATA_BUFF_LEN+1;
			}
		}
		
		//����ͷ���ҵ��󣬾Ϳ�ʼ����������
		if(is_find_head)//���Ѿ��ҵ�����ͷ��
		{
			printf("333is_find_head==%d\r\n",is_find_head);
			if((rmote_buff[rmote_head_index] == PROTOCOL_TAIL_2)&&
			   (rmote_buff[(rmote_head_index+R_DATA_BUFF_MASK)&R_DATA_BUFF_MASK] == PROTOCOL_TAIL_1))//�ҵ���2β��
			{
			    	is_find_head = 0;
				printf("444is_find_head==%d\r\n",is_find_head);
				printf("we find it!!\r\n");
				
				if(rmote_head_index>rmote_rep_index)//���ܵ�ֵû��ѭ������Բ�ζ���ֵ
				{
		   	        		//memcpy(fram_data_buff,rmote_buff+rmote_rep_index+2,rmote_head_index-rmote_rep_index-3);
					//fram_len = rmote_head_index-rmote_rep_index-3;
					rmote_rep_index = R_DATA_BUFF_LEN+1;
				}
				else if(rmote_head_index<rmote_rep_index)//����ֵ����ѭ�����У���Ҫ����
				{
				    	if(rmote_rep_index<(R_DATA_BUFF_MASK-1))
					{
   			            		//memcpy(fram_data_buff,rmote_buff+((rmote_rep_index+2)&R_DATA_BUFF_MASK),R_DATA_BUFF_MASK-rmote_rep_index-1);
						if(rmote_head_index>1)
						{
							//memcpy(fram_data_buff+R_DATA_BUFF_MASK-rmote_rep_index-1,rmote_buff,rmote_head_index-1);
						}
	        			    			
				 	    	//fram_len = R_DATA_BUFF_MASK-rmote_rep_index+rmote_head_index-2;
						rmote_rep_index = R_DATA_BUFF_LEN+1;
					}
					else if(rmote_rep_index == (R_DATA_BUFF_MASK-1))
					{
        			    			//memcpy(fram_data_buff,rmote_buff,rmote_head_index-1);
				 	   	//fram_len = rmote_head_index-1;
						rmote_rep_index = R_DATA_BUFF_LEN+1;
					}
					else 
					{
	        					//memcpy(fram_data_buff,rmote_buff+1,rmote_head_index-2);
				 	    	//fram_len = rmote_head_index-2;
						rmote_rep_index = R_DATA_BUFF_LEN+1;
					}
				}
				return 1;//������ɺ��˳�
			}
			else
			{
				//�����ܵ���ֵ����������ݰ�ֵ��û�ҵ���βʱ���Ͷ�ʧ�˰�
				//����������Լ�����ģ�ע���ʽ���������
				if(((rmote_head_index - rmote_rep_index+R_DATA_BUFF_LEN)&R_DATA_BUFF_MASK) >32 )//MAX_FRAM_LEN
				{
				    	//��Ϊ����Ч����ͷ�ˣ���������ͷ
					is_find_head = 0;
					if(rmote_head_index>rmote_rep_index)
					{
						printf("444\r\n");
						rmote_rep_index = R_DATA_BUFF_LEN+1;
					}
					else if(rmote_head_index<rmote_rep_index)
					{
						printf("555\r\n");
						rmote_rep_index = R_DATA_BUFF_LEN+1;
					}

			    	}
			}


		}
		rmote_head_index++;
		rmote_head_index &= R_DATA_BUFF_MASK;
	}

	//printf("rmote_head_index==%d\r\n",rmote_head_index);
	for(i=0;i<rmote_head_index;i++)
	{
		printf("rmote_buff[%d]==%d",i,rmote_buff[i]);
		printf("==%x\r\n",rmote_buff[i]);
	}
	return 0;
}







