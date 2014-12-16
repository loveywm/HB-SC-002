/*
********************************************************************************

�����������豸�Ĵ���
ʹ�õ��Ƕ�ʱ��1�ĵ�һ����ͨ��

ʹ�õı������ͺ�:TEPS3806-100B-P526


********************************************************************************
*/

#include "system.h"
#include "drive.h"


//20141213
#define ENCODER_TIMER   TIM1  // Encoder unit connected to TIM3
#define ENCODER_PPR           (u16)(100)   // number of pulses per revolution
#define SPEED_BUFFER_SIZE 8

#define COUNTER_RESET   (u16)0
#define ICx_FILTER      (u8) 6 // 6<-> 670nsec

#define TIMx_PRE_EMPTION_PRIORITY 1
#define TIMx_SUB_PRIORITY 0



//#define SPEED_SAMPLING_FREQ (u16)(2000/(SPEED_SAMPLING_TIME+1))

/* Private variables ---------------------------------------------------------*/
static s16 hPrevious_angle, hSpeed_Buffer[SPEED_BUFFER_SIZE], hRot_Speed;
static u8 bSpeed_Buffer_Index = 0;
static volatile u16 hEncoder_Timer_Overflow; 
static bool bIs_First_Measurement = 1;


#define ENCODER_TIM_PERIOD 0xffff//���ֵԤ��Ƶ��65536-1
#define MAX_COUNT          10000//10000Ҳ����1ms�ڲ��ᳬ��10000������

//#define ENCODER_TIM_PERIOD ENCODER_PPR*4
//#define MAX_COUNT          200

//u16   CurrentCount=0;

/*******************************************************************************
* Function Name  : ENC_Clear_Speed_Buffer
* Description    : Clear speed buffer used for average speed calculation  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ENC_Clear_Speed_Buffer(void)
{   
        u32 i;

        for (i=0;i<SPEED_BUFFER_SIZE;i++)
        {
            hSpeed_Buffer[i] = 0;
        }
        bIs_First_Measurement = 1;
}


s16   Enc_GetCount(void)
{
        static  u16   lastCount = 0;
        u16  curCount = ENCODER_TIMER->CNT;//��ȡ����ֵ
        s32 dAngle = curCount - lastCount;
        if(dAngle >= MAX_COUNT)
        {
            dAngle -= ENCODER_TIM_PERIOD;
        }
        else if(dAngle < -MAX_COUNT)
        {
            dAngle += ENCODER_TIM_PERIOD;
        }
        lastCount = curCount;
        return (s16)dAngle;
}




void EncoderSystick1000Routine(void)
{

        
}

void InitializeEncoder(void)
{
        
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        TIM_ICInitTypeDef TIM_ICInitStructure;

        /* Encoder unit connected to TIM1, 4X mode */    
        GPIO_InitTypeDef GPIO_InitStructure;
        //NVIC_InitTypeDef NVIC_InitStructure;

        /* TIM1 clock source enable */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//ʹ��TIM1ʱ��
        /* Enable GPIOA, clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

        GPIO_StructInit(&GPIO_InitStructure);
        /* Configure PA.06,07 as encoder input */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA8 PA9��������
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* Enable the TIM1 Update Interrupt */
        //NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
        //NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
        //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIMx_PRE_EMPTION_PRIORITY;
        //NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIMx_SUB_PRIORITY;
        //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        //NVIC_Init(&NVIC_InitStructure);

        /* Timer configuration in Encoder mode */
        TIM_DeInit(ENCODER_TIMER);
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

        TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling //�趨��������Ƶϵ��Ϊ0������Ƶ
        //TIM_TimeBaseStructure.TIM_Period = (4*ENCODER_PPR)-1;  //�趨��������װֵ 
        TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD-1;
        
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//����ʱ�ӷָ� T_dts = T_ck_int 
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM���ϼ���
        TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);

        TIM_EncoderInterfaceConfig(ENCODER_TIMER, TIM_EncoderMode_TI12, 
                                 TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3�������½�������
        TIM_ICStructInit(&TIM_ICInitStructure);
        TIM_ICInitStructure.TIM_ICFilter =ICx_FILTER;//ѡ������Ƚ��˲���
        TIM_ICInit(ENCODER_TIMER, &TIM_ICInitStructure);

        // Clear all pending interrupts
        TIM_ClearFlag(ENCODER_TIMER, TIM_FLAG_Update);//���TIM1�ĸ��±�־λ
        TIM_ITConfig(ENCODER_TIMER, TIM_IT_Update, ENABLE);

        //ENC_Clear_Speed_Buffer();

        //Reset counter
        TIM1->CNT = 0;
        //CurrentCount = TIM1->CNT;

        TIM_Cmd(ENCODER_TIMER, ENABLE);


        System.Device.Encoder.Enc_GetCount = Enc_GetCount;
}




