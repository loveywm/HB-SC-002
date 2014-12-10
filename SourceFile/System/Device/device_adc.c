/*
********************************************************************************
*                                Ƕ��ʽ΢ϵͳ
*                         
********************************************************************************
*�ļ���     : device_adc.c
*����       : ADC�豸
*ԭ��       : Adc�豸���ܿ飬����SystemTick����ʵ��ʵʱ���ݲ���
*           : ����ע�ắ���������������ݸ��ϲ�Ӧ�ñ���                  
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/01       ��ʼ�汾
********************************************************************************
*/

#include "drive.h"
#include "system.h"



static ushort NullValue;

static ushort * RegisterPointerBlock[4] =
{
    &NullValue, &NullValue, &NullValue, &NullValue
};


/*******************************************************************************
* ������	: Register
* ����	    : ��Ӧ�ò����ָ��ע�ᵽ��Ӧ��ͨ�������У�ʵ�ֵײ������򶥲㴫��
* �������  : adcChannel: ADCͨ����
*           : dataPoint: Ӧ�ò����ָ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/08       ��ʼ�汾
*******************************************************************************/
static void Register(AdcChannelEnum adcChannel, ushort * dataPointer)
{
    RegisterPointerBlock[adcChannel] = dataPointer;
}


/*******************************************************************************
* ������	: AdcSystick10000Routine
* ����	    : ϵͳʱ��10000/S����0.1mS�������һ�����У���ȡADCֵ��App�����
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/10       ��ʼ�汾
*******************************************************************************/
void AdcSystick10000Routine(void)
{
    static byte Counter = 0;
    
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//�ȴ�ת������
    *RegisterPointerBlock[Counter] = ADC_GetConversionValue(ADC1);

    //if (++Counter > 3) Counter = 0;
    
    ADC_RegularChannelConfig(ADC1,0,1,ADC_SampleTime_239Cycles5);//���ù�������ͨ���Լ���������

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ��ָ���� ADC1 �����ת����������
}

void InitializeAdc(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        ADC_InitTypeDef ADC_InitStructure;

        //ʹ��ADC1��ͨ��0,��Ӧ����PA0
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);//���÷�Ƶ����λ 6��ʱ��Ϊ 72/6=12MHz
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        ADC_DeInit(ADC1);//ADC ʱ�Ӹ�λ

        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC ��ģʽ:����ģʽ
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;//�����Ƿ���ɨ��ģʽ
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�����Ƿ�������ת��ģʽ
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//������������ת����ת�����ⲿ�¼�
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//���� ADC ���ݶ��뷽ʽ������뻹���Ҷ���
        ADC_InitStructure.ADC_NbrOfChannel = 1;//˳����й���ת���� ADC ͨ������Ŀ 1
        ADC_Init(ADC1, &ADC_InitStructure);

        ADC_Cmd(ADC1, ENABLE);//ʹ��ָ���� ADC1

        ADC_ResetCalibration(ADC1);//ִ�и�λУ׼

        while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ���λУ׼����

        ADC_StartCalibration(ADC1);//��ʼָ�� ADC1 ��У׼״̬

        while(ADC_GetCalibrationStatus(ADC1));//�ȴ�У AD ׼����

        ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ��ָ���� ADC1 �����ת����������

        System.Device.Adc.Register = Register;
}

