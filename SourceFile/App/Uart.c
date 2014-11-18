#include "system.h"
#include "app.h"


//串口循环队列接受方式
#define UART3_RX_BUFFER_SIZE  256
#define UART3_RX_BUFFER_MASK  255
static unsigned char UART3_RxBuf[UART3_RX_BUFFER_SIZE];
static volatile unsigned char UART3_RxHead = 0;
static volatile unsigned char UART3_RxTail = 0;

//供其他层调用，基本上是串口接受中断后传递值参数给data
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

///////////////////////////////////帧描述
//帧头
#define   PROTOCOL_HEAD_1       0x5A
#define   PROTOCOL_HEAD_1_COD   0xA5
#define   PROTOCOL_HEAD_2       0x55

//帧尾
#define   PROTOCOL_TAIL_1       0x6A
#define   PROTOCOL_TAIL_1_COD   0x95
#define   PROTOCOL_TAIL_2       0x69

/////////////////////////////////////////////
#define    R_DATA_BUFF_LEN  256    //之所以要定义256个长度的数组，就是为了能够让数组“首尾相接”。因为0 -1 = 255 ， 255+1 = 0。
#define    R_DATA_BUFF_MASK 255
unsigned char rmote_buff[R_DATA_BUFF_LEN];
unsigned int rmote_head_index = 0;//存储数据的指针
unsigned int rmote_rep_index  = R_DATA_BUFF_LEN + 1;//头位置
unsigned char is_find_head = 0;

unsigned char   Rcv_GPRS_Cmd(void)
{
    	int  i = 0;
	rmote_head_index = 0;
    	while(ReceiveByte3(&rmote_buff[rmote_head_index]))
  	{
		//第一次寻找头
		if((rmote_rep_index == R_DATA_BUFF_LEN+1)&&(rmote_buff[rmote_head_index] == PROTOCOL_HEAD_1))//找到头了
		{ 
		 	rmote_rep_index = rmote_head_index;
			printf("111rmote_head_index==%d\r\n",rmote_head_index);
			printf("111rmote_rep_index==%d\r\n",rmote_rep_index);
		}
		
		//第二次找头
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
		
		//两个头都找到后，就开始处理数据了
		if(is_find_head)//若已经找到报文头了
		{
			printf("333is_find_head==%d\r\n",is_find_head);
			if((rmote_buff[rmote_head_index] == PROTOCOL_TAIL_2)&&
			   (rmote_buff[(rmote_head_index+R_DATA_BUFF_MASK)&R_DATA_BUFF_MASK] == PROTOCOL_TAIL_1))//找到第2尾了
			{
			    	is_find_head = 0;
				printf("444is_find_head==%d\r\n",is_find_head);
				printf("we find it!!\r\n");
				
				if(rmote_head_index>rmote_rep_index)//接受的值没有循环超过圆形队列值
				{
		   	        		//memcpy(fram_data_buff,rmote_buff+rmote_rep_index+2,rmote_head_index-rmote_rep_index-3);
					//fram_len = rmote_head_index-rmote_rep_index-3;
					rmote_rep_index = R_DATA_BUFF_LEN+1;
				}
				else if(rmote_head_index<rmote_rep_index)//接受值过了循环队列，需要处理
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
				return 1;//接受完成后退出
			}
			else
			{
				//当接受的数值超过最大数据包值还没找到结尾时，就丢失此包
				//这个长度是自己定义的，注意等式后面的数字
				if(((rmote_head_index - rmote_rep_index+R_DATA_BUFF_LEN)&R_DATA_BUFF_MASK) >32 )//MAX_FRAM_LEN
				{
				    	//认为是无效报文头了，则重新找头
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








