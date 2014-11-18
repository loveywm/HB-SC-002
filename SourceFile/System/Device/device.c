
#include "drive.h"
#include "system.h"

extern void InitializeKey(void);

//extern void InitializeLcd(void);

extern void InitializeRtc(void);

extern void InitializeTimer(void);

extern void InitializeUsart(void);

extern void InitializeSystick(void);

extern void InitializeIO(void);

extern void InitializeAdc(void);

//extern void InitializeStorage(void);

extern void InitializeDevice(void);



/*******************************************************************************
* ������	: InitializeDevice
* ����	    : ��ʼ���豸
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/08       ��ʼ�汾
*******************************************************************************/
void InitializeDevice(void)
{
    RCC->APB2ENR |=  RCC_APB2Periph_GPIOA \
                    |RCC_APB2Periph_GPIOB \
                    |RCC_APB2Periph_GPIOC \
                    |RCC_APB2Periph_GPIOD \
                    |RCC_APB2Periph_GPIOE \
                    |RCC_APB2Periph_GPIOF \
                    |RCC_APB2Periph_GPIOG\
                    |RCC_APB2Periph_AFIO;

    InitializeUsart();	

    //InitializeKey();	

    //InitializeLcd();

    InitializeRtc();

    InitializeTimer();

    InitializeIO();

    InitializeAdc();

    //InitializeStorage();

    InitializeSystick();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
	NVIC_SetPriority(SysTick_IRQn, 0xFE);   //�����ȼ�
 	NVIC_SetPriority(PendSV_IRQn, 0xFF);    //������ȼ�
    
}


